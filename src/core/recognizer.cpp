#include "recognizer.h"

#include <QDebug>
#include <QLibrary>

Recognizer::Recognizer(const QString& proxyLib)
{
    qDebug() << "Loading library" << proxyLib;
    _lib = new QLibrary(proxyLib);
    if (!_lib->load())
    {
        qCritical() << "Unable to load libray" << _lib->errorString();
        return;
    }
    dnnPrepare = (DnnPrepare)resolve("ck_dnn_proxy__prepare");
    dnnRecognize = (DnnRecognize)resolve("ck_dnn_proxy__recognize");
    qDebug() << "OK";
}

Recognizer::~Recognizer()
{
    if (_lib)
    {
        if (_lib->isLoaded())
            _lib->unload();
        delete _lib;
    }
}

bool Recognizer::ready() const
{
    return _lib && _lib->isLoaded() && dnnPrepare && dnnRecognize;
}

QFunctionPointer Recognizer::resolve(const char* symbol)
{
    auto func = _lib->resolve(symbol);
    if (!func)
        qCritical() << "FAILED: Unable to resolve" << symbol << _lib->errorString();
    return func;
}

void Recognizer::prepare(const QString &modelFile, const QString &weightsFile,
                         const QString &meanFile, const QString &labelFile)
{
    ck_dnn_proxy__init_param p;
    p.model_file = modelFile.toUtf8().data();
    p.trained_file = weightsFile.toUtf8().data();
    p.mean_file = meanFile.toUtf8().data();
    p.label_file = labelFile.toUtf8().data();
    dnnPrepare(&p);
}

ExperimentProbe Recognizer::recognize(const QString& imageFile)
{
    ExperimentProbe probe;
    ck_dnn_proxy__recognition_param param;
    param.image_file = imageFile.toUtf8().data();
    ck_dnn_proxy__recognition_result result;
    dnnRecognize(&param, &result);
    probe.image = imageFile;
    probe.time = result.time;
    probe.memory = result.memory;
    for (int i = 0; i < PREDICTIONS_COUNT; i++)
    {
        PredictionResult p;
        p.probability = result.predictions[i].accuracy;
        p.id = QString::number(result.predictions[i].index);
        probe.predictions << p;
    }
    return probe;
}
