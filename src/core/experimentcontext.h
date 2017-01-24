#ifndef EXPERIMENTCONTEXT_H
#define EXPERIMENTCONTEXT_H

#include "appmodels.h"

#include <QObject>
#include <QList>

class PlatformFeaturesProvider;
class ScenariosProvider;

class ExperimentContext : public QObject
{
    Q_OBJECT

public:
    int experimentIndex = -1;
    PlatformFeaturesProvider* platformFeaturesProvider;
    ScenariosProvider* scenariosProvider;

    bool checkScenarioIndex(int index) const;

    bool currentScenarioExists() const;
    int currentScenarioIndex() const { return _currentScenarioIndex; }
    void setCurrentScenarioIndex(int index);
    const RecognitionScenario& currentScenario() const;

    const QList<RecognitionScenario>& currentScenarios() const;

    int batchSize() const { return _batchSize; }
    void setBatchSize(int value);
    int minBatchSize() const { return 2; }
    int maxBatchSize() const { return 16; }

    bool isExperimentStarted() const { return _isExperimentStarted; }

    void startExperiment();
    void stopExperiment();

    void loadFromConfig();

signals:
    void experimentStarted();
    void experimentStopped();

private:
    bool _isExperimentStarted = false;
    int _currentScenarioIndex = -1;
    int _batchSize = 2;
};

#endif // EXPERIMENTCONTEXT_H
