#include "recognizer.h"

#include <QDebug>
#include <QLibrary>

//extern "C"
//{
//void ck_prepare(const char *model_file, const char *trained_file, const char *mean_file, const char *label_file);
//}

Recognizer::Recognizer(const QString& proxyLib,
                       const QString& modelFile,
                       const QString& weightFile,
                       const QString& meanFile,
                       const QString& labelFile)
{
    qDebug() << "Loading library" << proxyLib;
    _lib = new QLibrary(proxyLib);
    if (!_lib->load())
    {
        qCritical() << "Unable to load libray" << _lib->errorString();
        return;
    }
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
    return _lib && _lib->isLoaded();
}

QVector<PredictionResult> Recognizer::recognize(const QString& imageFile)
{

}
