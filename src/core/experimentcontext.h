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
    int experimentIndex() const { return _experimentIndex; }
    ScenariosProvider* scenariosProvider;

    bool checkScenarioIndex(int index) const;
    bool currentScenarioExists() const;
    int currentScenarioIndex() const { return _currentScenarioIndex; }
    void setCurrentScenarioIndex(int index);
    const RecognitionScenario& currentScenario() const;
    const QList<RecognitionScenario>& currentScenarios() const;

    int batchSize() const { return _batchSize; }
    void setBatchSize(int value);
    int minBatchSize() const { return 1; }
    int maxBatchSize() const { return 16; }

    void startExperiment();
    void stopExperiment();
    bool isExperimentStarted() const { return _isExperimentStarted; }
    const ExperimentResult& experimentResult() const { return _result; }

    void loadFromConfig();

signals:
    void experimentStarted();
    void experimentStopping();
    void experimentFinished();
    void experimentResultReady();

public slots:
    void recognitionFinished(const ExperimentProbe& p);

private:
    int _experimentIndex = -1;
    bool _isExperimentStarted = false;
    int _currentScenarioIndex = -1;
    int _batchSize = 2;
    ExperimentResult _result;

    friend class MainWindow;
};

#endif // EXPERIMENTCONTEXT_H
