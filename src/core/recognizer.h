#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QString>
#include <QVector>

QT_BEGIN_NAMESPACE
class QLibrary;
QT_END_NAMESPACE

#include "appmodels.h"

struct ck_dnn_proxy__recognition_result
{
    double time;
    double memory;
};

typedef void (*DnnPrepare)(const char *model_file, const char *trained_file,
                           const char *mean_file, const char *label_file);
typedef int (*DnnRecognize)(const char *image_file, ck_dnn_proxy__recognition_result* result);


class Recognizer
{
public:
    Recognizer(const QString &proxyLib);
    ~Recognizer();

    void prepare(const QString &modelFile, const QString &weightsFile,
                 const QString &meanFile, const QString &labelFile);

    ExperimentProbe recognize(const QString& imageFile);

    bool ready() const;

private:
    QLibrary* _lib = nullptr;

    DnnPrepare dnnPrepare;
    DnnRecognize dnnRecognize;

    QFunctionPointer resolve(const char* symbol);
};

#endif // RECOGNIZER_H
