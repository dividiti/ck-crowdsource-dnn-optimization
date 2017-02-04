#include "appconfig.h"
#include "appevents.h"
#include "recognizer.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLibrary>

PredictionLabel::PredictionLabel(const QString& line)
{
    int pos = line.indexOf(' ');
    if (pos > 0)
        index = line.left(pos);
    label = line.right(line.length()-pos-1);
}

//-----------------------------------------------------------------------------

Recognizer::Recognizer(const QString& proxyLib)
{
    qDebug() << "Loading library" << proxyLib;
    _lib = new QLibrary(proxyLib);
    if (!_lib->load())
    {
        AppEvents::error(_lib->errorString());
        return;
    }
    dnnPrepare = (DnnPrepare)resolve("ck_dnn_proxy__prepare");
    dnnRecognize = (DnnRecognize)resolve("ck_dnn_proxy__recognize");
    dnnRelease = (DnnRelease)resolve("ck_dnn_proxy__release");
    qDebug() << "OK";
}

Recognizer::~Recognizer()
{
    if (_lib)
    {
        if (_dnnHandle)
            dnnRelease(_dnnHandle);
        if (_lib->isLoaded())
            _lib->unload();
        delete _lib;
    }
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
    char* data = new char[bytes.length()];
    memcpy(data, bytes.data(), bytes.length());
    return data;
}

void Recognizer::prepare(const QString &modelFile, const QString &weightsFile,
                         const QString &meanFile, const QString &labelsFile)
{
    ck_dnn_proxy__init_param p;
    p.model_file = makeLocalStr(modelFile);
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
