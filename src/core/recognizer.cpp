#include "appevents.h"
#include "recognizer.h"

#include <QDebug>
#include <QFile>
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
    _dnnHandle = dnnPrepare(&p);
    delete[] p.model_file;
    delete[] p.trained_file;
    delete[] p.mean_file;

    loadLabels(labelsFile);
}

void Recognizer::recognize(const QString& imageFile, ExperimentProbe& probe)
{
    //const char *image = "/home/kolyan/Projects/crowdsource-video-experiments-on-desktop/images/sample1.jpg";

    ck_dnn_proxy__recognition_param param;
    param.proxy_handle = _dnnHandle;
    param.image_file = makeLocalStr(imageFile);

    ck_dnn_proxy__recognition_result result;
    dnnRecognize(&param, &result);
    delete[] param.image_file;
    probe.image = imageFile;
    probe.time = result.time;
    probe.memory = result.memory;
    if (probe.predictions.capacity() < PREDICTIONS_COUNT)
        probe.predictions.resize(PREDICTIONS_COUNT);
    for (int i = 0; i < PREDICTIONS_COUNT; i++)
    {
        probe.predictions[i].accuracy = result.predictions[i].accuracy;
        probe.predictions[i].index = QString(); // TODO separate nXXX from labels

        int index = result.predictions[i].index;
        if (index >= 0 && index < _labels.size())
            probe.predictions[i].labels = _labels.at(index);
    }
}

void Recognizer::loadLabels(const QString& fileName)
{
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
            _labels << in.readLine();
       inputFile.close();
    }
}
