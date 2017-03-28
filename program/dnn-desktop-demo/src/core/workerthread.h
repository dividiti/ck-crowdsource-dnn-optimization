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
    WorkerThread(const Program& program, const Model& model, const Dataset& dataset, int batchSize, const Mode& mode, QObject* parent = Q_NULLPTR);

    void run() override;

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
};

#endif // WORKERTHREAD_H

