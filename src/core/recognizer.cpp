#include "appconfig.h"
#include "appevents.h"
#include "recognizer.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLibrary>

PredictionLabel::PredictionLabel(const QString& line)
{
    int pos = line.indexOf(' ');
    if (pos > 0)
        index = line.left(pos);
    label = line.right(line.length()-pos-1);
}

//-----------------------------------------------------------------------------

namespace LibraryPaths
{
#ifdef Q_OS_WIN32
    const QByteArray pathVar("PATH");
    const char pathSepator = ';';
#endif
#ifdef Q_OS_LINUX
    const QByteArray pathVar("LD_LIBRARY_PATH");
    const char pathSepator = ':';
#endif
#ifdef Q_OS_MAC
    const QByteArray pathVar("DYLD_FALLBACK_LIBRARY_PATH");
    const char pathSepator = ':';
#endif

QByteArray set(const QStringList& paths)
{
    auto oldValue = qgetenv(pathVar);
    auto newValue = paths.join(pathSepator).toUtf8();
    if (!oldValue.isEmpty())
        newValue += pathSepator + oldValue;
    if (!qputenv(pathVar, newValue))
        qWarning() << "Unable to set env var" << pathVar << "to value" << newValue;
    qDebug() << pathVar << qgetenv(pathVar);
    return oldValue;
}

void set(const QByteArray& paths)
{
    qputenv(pathVar, paths);
}

} // namespace LibraryPaths

//-----------------------------------------------------------------------------

Recognizer::Recognizer(const QString& proxyLib, const QStringList &depLibs)
{
    qDebug() << "Loading library" << proxyLib;

    // Settings LD_LIBRARY_PATH inside of app does not help to search all dependencies
    // (although when we set LD_LIBRARY_PATH in console before app is run, it helps).
    // At least on Ubuntu. So we have to load all deps directly.
    for (int i = depLibs.size()-1; i >= 0; i--)
    {
        auto depLib = depLibs.at(i);
        if (!QFile(depLib).exists())
            continue;
        qDebug() << "Load dep lib" << depLib;
        auto lib = new QLibrary(depLib);
        if (!lib->load())
        {
            AppEvents::error(lib->errorString());
            release();
            return;
        }
        _deps << lib;
    }

    _lib = new QLibrary(proxyLib);
    if (!_lib->load())
    {
        AppEvents::error(_lib->errorString());
        release();
        return;
    }
    dnnPrepare = (DnnPrepare)resolve("ck_dnn_proxy__prepare");
    dnnRecognize = (DnnRecognize)resolve("ck_dnn_proxy__recognize");
    dnnRelease = (DnnRelease)resolve("ck_dnn_proxy__release");
    qDebug() << "OK";
}

Recognizer::~Recognizer()
{
    release();
}

void Recognizer::release()
{
    for (int i = _deps.size()-1; i >= 0; i--)
    {
        if (_deps.at(i)->isLoaded())
            _deps.at(i)->unload();
        delete _deps.at(i);
    }
    _deps.clear();

    if (_lib)
    {
        if (_dnnHandle)
        {
            dnnRelease(_dnnHandle);
            _dnnHandle = nullptr;
        }
        if (_lib->isLoaded())
            _lib->unload();
        delete _lib;
        _lib = nullptr;
    }

    if (!_tmpModelFile.isEmpty())
        QFile(_tmpModelFile).remove();
}

bool Recognizer::ready() const
{
    return _lib && _lib->isLoaded() && dnnPrepare && dnnRecognize && dnnRelease;
}

QFunctionPointer Recognizer::resolve(const char* symbol)
{
    auto func = _lib->resolve(symbol);
    if (!func)
        AppEvents::error(_lib->errorString());
    return func;
}

char* makeLocalStr(const QString& s)
{
    auto bytes = s.toUtf8();
    char* data = new char[bytes.length()+1];
    memcpy(data, bytes.data(), bytes.length());
    data[bytes.length()] = '\0';
    return data;
}

bool Recognizer::prepare(const QString &modelFile, const QString &weightsFile,
                         const QString &meanFile, const QString &labelsFile)
{
    if (!checkFileExists(modelFile)) return false;
    if (!checkFileExists(weightsFile)) return false;
    if (!checkFileExists(meanFile)) return false;
    if (!checkFileExists(labelsFile)) return false;

    _tmpModelFile = prepareModelFile(modelFile);
    if (_tmpModelFile.isEmpty()) return false;

    ck_dnn_proxy__init_param p;
    p.model_file = makeLocalStr(_tmpModelFile);
    p.trained_file = makeLocalStr(weightsFile);
    p.mean_file = makeLocalStr(meanFile);

    // TODO: caffe uses glog, but it seems that we can't ask it if it's initialized or not
    // (https://github.com/baysao/google-glog/issues/113) but second initialization leads to fail
    static bool nasty_hack_for_if_glog_initialized = false;
    if (!nasty_hack_for_if_glog_initialized)
    {
        p.logs_path = makeLocalStr(AppConfig::logPath());
        nasty_hack_for_if_glog_initialized = true;
    }
    else p.logs_path = nullptr;

    _dnnHandle = dnnPrepare(&p);
    // TODO: process errors
    delete[] p.model_file;
    delete[] p.trained_file;
    delete[] p.mean_file;
    if (p.logs_path)
        delete[] p.logs_path;

    loadLabels(labelsFile);
    return true;
}

void Recognizer::recognize(const QString& imageFile, ExperimentProbe& probe)
{
    ck_dnn_proxy__recognition_param param;
    param.proxy_handle = _dnnHandle;
    param.image_file = makeLocalStr(imageFile);

    ck_dnn_proxy__recognition_result result;
    dnnRecognize(&param, &result);
    delete[] param.image_file;

    probe.image = imageFile;
    probe.time = result.duration;
    probe.memory = result.memory_usage;
    if (probe.predictions.capacity() < PREDICTIONS_COUNT)
        probe.predictions.resize(PREDICTIONS_COUNT);
    for (int i = 0; i < PREDICTIONS_COUNT; i++)
    {
        probe.predictions[i].accuracy = result.predictions[i].accuracy;

        int index = result.predictions[i].index;
        if (index >= 0 && index < _labels.size())
        {
            auto label = _labels.at(index);
            probe.predictions[i].labels = label.label;
            probe.predictions[i].index = label.index;
        }
    }
}

void Recognizer::loadLabels(const QString& fileName)
{
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
            _labels << PredictionLabel(in.readLine());
       inputFile.close();
    }
}

bool Recognizer::checkFileExists(const QString& fileName)
{
    bool res = QFile(fileName).exists();
    if (!res)
        AppEvents::error("File not found: " + fileName);
    return res;
}

QString Recognizer::prepareModelFile(const QString& fileName)
{
    qDebug() << "Prepare model file" << fileName;
    auto text = Utils::loadTtextFromFile(fileName);
    if (text.isEmpty()) return QString();
    text = text.replace("$#batch_size#$", "1");
    auto tmpFile = Utils::makePath({ AppConfig::tmpPath(), "tmp.caffemodel" });
    if (!Utils::saveTextToFile(tmpFile, text)) return QString();
    return tmpFile;
}
