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
    void notifyModeChanged(const Mode& mode);
    bool isExperimentStarted() const { return _isExperimentStarted; }

signals:
    void experimentStarted();
    void experimentStopping();
    void experimentFinished();
    void newImageResult(ImageResult);
    void modeChanged(Mode);

private:
    bool _isExperimentStarted = false;
};

#endif // EXPERIMENTCONTEXT_H
