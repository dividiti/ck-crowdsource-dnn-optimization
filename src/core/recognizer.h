#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QString>

QT_BEGIN_NAMESPACE
class QLibrary;
QT_END_NAMESPACE

#include "appmodels.h"

//-----------------------------------------------------------------------------

#define PREDICTIONS_COUNT 5

struct ck_dnn_proxy__init_param {
    const char *model_file;
    const char *trained_file;
    const char *mean_file;
    const char *logs_path;
};

struct ck_dnn_proxy__recognition_param {
    void* proxy_handle;
    const char* image_file;
};

struct ck_dnn_proxy__recognition_result {
    int status;
    double start_time;
    double duration;
    double memory_usage;
    struct {
        float accuracy;
        int index;
    } predictions[PREDICTIONS_COUNT];
};

typedef void* (*DnnPrepare)(ck_dnn_proxy__init_param*);
typedef void (*DnnRecognize)(ck_dnn_proxy__recognition_param*,
                             ck_dnn_proxy__recognition_result*);
typedef void (*DnnRelease)(void*);

//-----------------------------------------------------------------------------

class Recognizer
{
public:
    Recognizer(const QString &proxyLib, const QStringList& depLibs);
    ~Recognizer();

    bool prepare(const QString &modelFile, const QString &weightsFile,
                 const QString &meanFile, const QString &labelsFile);

    void recognize(const QString& imageFile, ExperimentProbe& probe);

    bool ready() const { return _ready; }

    static int predictionsCount() { return PREDICTIONS_COUNT; }

    QString predictionLabel(int predictionIndex) const;

private:
    QLibrary* _lib = nullptr;
    QList<QLibrary*> _deps;
    DnnPrepare dnnPrepare;
    DnnRecognize dnnRecognize;
    DnnRelease dnnRelease;
    void* _dnnHandle = nullptr;
    QStringList _labels;
    QString _tmpModelFile;
    bool _ready = false;

    void release();
    QString loadDeps(const QStringList &depLibs);
    bool loadLabels(const QString &labelsFile);

    static char* makeLocalStr(const QString& s);
    static bool checkFileExists(const QString& fileName);
    static QString prepareModelFile(const QString& fileName);
    static char* prepareLogging();
};

#endif // RECOGNIZER_H
