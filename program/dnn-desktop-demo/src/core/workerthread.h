#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "appmodels.h"
#include <QThread>

class WorkerThread : public QThread
{
    Q_OBJECT
public:

    void run() override;

signals:
    void newImageResult(ImageResult result);
    void stopped();

private:
    void processPredictedResults(const ImageResult& imageResult);
};

#endif // WORKERTHREAD_H

