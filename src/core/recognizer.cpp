#include "appconfig.h"
#include "appevents.h"
#include "recognizer.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLibrary>

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
    try
    {
        qDebug() << "Loading dependencies. Count:" << depLibs.size();
        auto res = loadDeps(depLibs);
        if (!res.isEmpty()) throw res;

        qDebug() << "Loading main library" << proxyLib;
        _lib = new QLibrary(proxyLib);
        if (!_lib->load()) throw _lib->errorString();

        dnnPrepare = (DnnPrepare)_lib->resolve("ck_dnn_proxy__prepare");
        if (!dnnPrepare) throw _lib->errorString();

        dnnRecognize = (DnnRecognize)_lib->resolve("ck_dnn_proxy__recognize");
        if (!dnnRecognize) throw _lib->errorString();

        dnnRelease = (DnnRelease)_lib->resolve("ck_dnn_proxy__release");
        if (!dnnRelease) throw _lib->errorString();

        _ready = true;
        qDebug() << "OK";
    }
    catch (QString error)
    {
        AppEvents::error(error);
        release();
    }
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

QString Recognizer::loadDeps(const QStringList &depLibs)
{
    // Settings LD_LIBRARY_PATH inside of app does not help to search all dependencies
    // (although when we set LD_LIBRARY_PATH in console before app is run, it helps).
    // At least on Ubuntu. So we have to load all deps directly.
    for (int i = depLibs.size()-1; i >= 0; i--)
    {
        auto depLib = depLibs.at(i);
        qDebug() << "Load dep lib" << i+1 << depLib;
        if (!QFile(depLib).exists())
        {
            qWarning() << "File not found";
            continue;
        }
        auto lib = new QLibrary(depLib);
        if (!lib->load())
            return lib->errorString();
        _deps << lib;
    }
    return QString();
}

char* Recognizer::makeLocalStr(const QString& s)
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
    if (!loadLabels(labelsFile)) return false;

    _tmpModelFile = prepareModelFile(modelFile);
    if (_tmpModelFile.isEmpty()) return false;

    ck_dnn_proxy__init_param p;
    p.model_file = makeLocalStr(_tmpModelFile);
    p.trained_file = makeLocalStr(weightsFile);
    p.mean_file = makeLocalStr(meanFile);
    p.logs_path = prepareLogging();
    _dnnHandle = dnnPrepare(&p);
    // TODO: process errors

    delete[] p.model_file;
    delete[] p.trained_file;
    delete[] p.mean_file;
    if (p.logs_path)
        delete[] p.logs_path;

    return true;
}

char* Recognizer::prepareLogging()
{
    // TODO: caffe uses glog, but it seems that we can't ask it if it's initialized or not
    // (https://github.com/baysao/google-glog/issues/113) but second initialization leads to fail
    static bool nasty_hack_for_if_glog_initialized = false;
    if (!nasty_hack_for_if_glog_initialized)
    {
        nasty_hack_for_if_glog_initialized = true;
        return makeLocalStr(AppConfig::logPath());
    }
    return nullptr;
}

void Recognizer::recognize(const ImageEntry& image, ExperimentProbe& probe)
{
    ck_dnn_proxy__recognition_param param;
    param.proxy_handle = _dnnHandle;
    param.image_file = makeLocalStr(image.fileName);

    ck_dnn_proxy__recognition_result result;
    dnnRecognize(&param, &result);
    delete[] param.image_file;

    probe.image = image.fileName;
    probe.time = result.duration;
    probe.memory = result.memory_usage;

    probe.correctInfo.index = image.correctIndex;
    probe.correctInfo.labels = predictionLabel(image.correctIndex);

    if (probe.predictions.capacity() < predictionsCount())
        probe.predictions.resize(predictionsCount());

    bool correctFound = false;
    for (int i = 0; i < predictionsCount(); i++)
    {
        PredictionResult& res = probe.predictions[i];
        res.index = result.predictions[i].index;
        res.labels = predictionLabel(res.index);
        res.isCorrect = res.index == image.correctIndex;
        res.accuracy = result.predictions[i].accuracy;
        if (res.isCorrect)
        {
            correctFound = true;
            probe.correctAsTop1 = i == 0;
            probe.correctAsTop5 = true;
            probe.correctInfo.accuracy = res.accuracy;
        }
    }
    if (!correctFound)
    {
        probe.correctAsTop1 = false;
        probe.correctAsTop5 = false;
        probe.correctInfo.accuracy = 0;
    }
    probe.correctInfo.isCorrect = probe.correctAsTop5;
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
    auto text = Utils::loadTextFromFile(fileName);
    if (text.isEmpty()) return QString();
    text = text.replace("$#batch_size#$", "2");
    auto tmpFile = Utils::makePath({ AppConfig::tmpPath(), "tmp.caffemodel" });
    if (!Utils::saveTextToFile(tmpFile, text)) return QString();
    return tmpFile;
}

bool Recognizer::loadLabels(const QString& labelsFile)
{
    QFile file(labelsFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        AppEvents::error(QString("Unable load labels from %1: %2")
                         .arg(labelsFile).arg(file.errorString()));
        return false;
    }
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        auto s = stream.readLine(); // trim nXXXXX indices:
        _labels << s.right(s.length() - s.indexOf(' ') - 1);
    }
    return true;
}

QString Recognizer::predictionLabel(int predictionIndex) const
{
    if (predictionIndex >= 0 && predictionIndex < _labels.size())
        return _labels.at(predictionIndex);
    return QString();
}
