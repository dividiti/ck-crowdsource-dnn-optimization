#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "appmodels.h"
#include <QThread>

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(const Program& program, const Model& model, const Dataset& dataset, QObject* parent = Q_NULLPTR);

    void run() override;

signals:
    void newImageResult(ImageResult result);
    void stopped();

private:
    void processPredictedResults(const ImageResult& imageResult);
    void emitStopped();

    Program program;
    Model model;
    Dataset dataset;
};

#endif // WORKERTHREAD_H

