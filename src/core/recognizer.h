#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QString>
#include <QVector>

QT_BEGIN_NAMESPACE
class QLibrary;
QT_END_NAMESPACE

#include "appmodels.h"

//-----------------------------------------------------------------------------

#define PREDICTIONS_COUNT 5

struct ck_dnn_proxy__init_param
{
    const char *model_file;
    const char *trained_file;
    const char *mean_file;
    const char *label_file;
};

struct ck_dnn_proxy__recognition_param
{
    const char* image_file;
};

struct ck_dnn_proxy__recognition_result
{
    double time;
    double memory;
    struct
    {
        double accuracy;
        int index;
    }
    predictions[PREDICTIONS_COUNT];
};

typedef void (*DnnPrepare)(ck_dnn_proxy__init_param *param);
typedef void (*DnnRecognize)(ck_dnn_proxy__recognition_param *param,
                             ck_dnn_proxy__recognition_result *result);

//-----------------------------------------------------------------------------

class Recognizer
{
public:
    Recognizer(const QString &proxyLib);
    ~Recognizer();

    void prepare(const QString &modelFile, const QString &weightsFile,
                 const QString &meanFile, const QString &labelFile);

    void recognize(const QString& imageFile, ExperimentProbe& probe);

    bool ready() const;

    int predictionsCount() const { return PREDICTIONS_COUNT; }

private:
    QLibrary* _lib = nullptr;

    DnnPrepare dnnPrepare;
    DnnRecognize dnnRecognize;

    QFunctionPointer resolve(const char* symbol);
};

#endif // RECOGNIZER_H
