#include "appconfig.h"
#include "appevents.h"
#include "libloader.h"
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
        if (!QFile(proxyLib).exists())
            throw "Lib not found: " + proxyLib;

        _lib = LibLoader::create();

        qDebug() << "Loading dependencies. Count:" << depLibs.size();
        _lib->loadDeps(depLibs);

        qDebug() << "Loading main library" << proxyLib;
        _lib->loadLib(proxyLib);
        dnnPrepare = (DnnPrepare)_lib->resolve("ck_dnn_proxy__prepare");
        dnnRecognize = (DnnRecognize)_lib->resolve("ck_dnn_proxy__recognize");
        dnnRelease = (DnnRelease)_lib->resolve("ck_dnn_proxy__release");

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

    if (_lib)
    {
        if (_dnnHandle)
        {
            dnnRelease(_dnnHandle);
            _dnnHandle = nullptr;
        }
        delete _lib;
        _lib = nullptr;
    }

    if (!_tmpModelFile.isEmpty())
        QFile(_tmpModelFile).remove();
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
    p.model_file = Utils::makeLocalStr(_tmpModelFile);
    p.trained_file = Utils::makeLocalStr(weightsFile);
    p.mean_file = Utils::makeLocalStr(meanFile);
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
        return Utils::makeLocalStr(AppConfig::logPath());
    }
    return nullptr;
}

void Recognizer::recognize(const ImageEntry& image, ExperimentProbe& probe)
{
    ck_dnn_proxy__recognition_param param;
    param.proxy_handle = _dnnHandle;
    param.image_file = Utils::makeLocalStr(image.fileName);

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
    text = text.replace("$#batch_size#$", "1");
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

