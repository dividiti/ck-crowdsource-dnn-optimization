#include "appevents.h"
#include "recognizer.h"

#include <QDebug>
#include <QLibrary>

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

void Recognizer::prepare(const QString &modelFile, const QString &weightsFile,
                         const QString &meanFile, const QString &labelFile)
{
    const char* model = "/home/nikolay/CK/ck-caffe/program/caffe-classification/tmp/tmp-9nNsmI.prototxt";
    const char* trained = "/home/nikolay/CK-TOOLS/caffemodel-bvlc-googlenet/bvlc_googlenet.caffemodel";
    const char *mean = "/home/nikolay/CK/ck-caffe/program/caffe-classification/imagenet_mean.binaryproto";
    const char* labels = "/home/nikolay/CK/ck-caffe/program/caffe-classification/synset_words.txt";
    ck_dnn_proxy__init_param p;
    p.model_file = model;//modelFile.toUtf8().data();
    p.trained_file = trained;//weightsFile.toUtf8().data();
    p.mean_file = mean;//meanFile.toUtf8().data();
    p.label_file = labels;//labelFile.toUtf8().data();
    _dnnHandle = dnnPrepare(&p);
}

void Recognizer::recognize(const QString& imageFile, ExperimentProbe& probe)
{
    const char *image = "/home/nikolay/Projects/crowdsource-video-experiments-on-desktop/images/sample1.jpg";

    ck_dnn_proxy__recognition_param param;
    param.proxy_handle = _dnnHandle;
    param.image_file = image;//imageFile.toUtf8();

    ck_dnn_proxy__recognition_result result;
    dnnRecognize(&param, &result);
    probe.image = imageFile;
    probe.time = result.time;
    probe.memory = result.memory;
    if (probe.predictions.capacity() < PREDICTIONS_COUNT)
        probe.predictions.resize(PREDICTIONS_COUNT);
    for (int i = 0; i < PREDICTIONS_COUNT; i++)
    {
        probe.predictions[i].accuracy = result.predictions[i].accuracy;
        probe.predictions[i].description  = QString(result.predictions[i].info.c_str());
    }
}
