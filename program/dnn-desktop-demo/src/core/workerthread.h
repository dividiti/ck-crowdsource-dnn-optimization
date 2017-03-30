#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "appmodels.h"
#include <QThread>

QT_BEGIN_NAMESPACE
class QStringList;
class QString;
QT_END_NAMESPACE

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(const Program& program, const Mode& mode, QObject* parent = Q_NULLPTR);

    void run() override;

    Mode getMode() const { return mode; }

    void setModel(const Model& model) { this->model = model; }
    void setDataset(const Dataset& dataset) { this->dataset = dataset; }
    void setBatchSize(int batchSize) { this->batchSize = batchSize; }
    void setMinResultInterval(long milliseconds) { minResultIntervalMs = milliseconds; }

signals:
    void newImageResult(ImageResult result);

private:
    void processPredictedResults(const ImageResult& imageResult);
    void emitStopped();

    QStringList getArgs();
    QString getOutputFile();

    Program program;
    Model model;
    Dataset dataset;
    int batchSize;
    Mode mode;
    long minResultIntervalMs = 0;
    qint64 lastResultMs = 0;
};

#endif // WORKERTHREAD_H

