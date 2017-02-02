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
    dnnPrepare(modelFile.toUtf8().data(), weightsFile.toUtf8().data(),
               meanFile.toUtf8().data(), labelFile.toUtf8().data());
}

ExperimentProbe Recognizer::recognize(const QString& imageFile)
{
    ExperimentProbe probe;
    ck_dnn_proxy__recognition_result result;
    int res = dnnRecognize(imageFile.toUtf8().data(), &result);
    if (res == 0)
    {
        probe.image = imageFile;
        probe.time = result.time;
        probe.memory = result.memory;
        qDebug() << "OK";
    }
    else
    {
        qCritical() << "FAILED";
    }
    return probe;
}
