#ifndef EXPERIMENTCONTEXT_H
#define EXPERIMENTCONTEXT_H

#include "appmodels.h"

#include <QObject>
#include <QList>

class ExperimentContext : public QObject
{
    Q_OBJECT

public:
    void startExperiment();
    void stopExperiment();
    bool isExperimentStarted() const { return _isExperimentStarted; }

signals:
    void experimentStarted();
    void experimentStopping();
    void experimentFinished();
    void newImageResult(ImageResult);

private:
    bool _isExperimentStarted = false;
};

#endif // EXPERIMENTCONTEXT_H
