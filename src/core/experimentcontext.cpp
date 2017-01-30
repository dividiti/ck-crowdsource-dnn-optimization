#include "appconfig.h"
#include "experimentcontext.h"
#include "scenariosprovider.h"

#include <QDebug>

bool ExperimentContext::checkScenarioIndex(int index) const
{
    return index >= 0 && index < currentScenarios().size();
}

bool ExperimentContext::currentScenarioExists() const
{
    return checkScenarioIndex(_currentScenarioIndex);
}

void ExperimentContext::setCurrentScenarioIndex(int index)
{
    if (checkScenarioIndex(index))
    {
        _currentScenarioIndex = index;
        AppConfig::setSelectedScenarioIndex(_experimentIndex, index);
    }
}

const RecognitionScenario& ExperimentContext::currentScenario() const
{
    return currentScenarios().at(_currentScenarioIndex);
}

const QList<RecognitionScenario>& ExperimentContext::currentScenarios() const
{
    return scenariosProvider->currentList().items();
}

void ExperimentContext::setBatchSize(int value)
{
    if (value >= minBatchSize() && value <= maxBatchSize())
    {
        _batchSize = value;
        AppConfig::setBatchSize(_experimentIndex, value);
    }
}

void ExperimentContext::startExperiment()
{
    _isExperimentStarted = true;
    _result.reset();
    emit experimentStarted();
}

void ExperimentContext::stopExperiment()
{
    _isExperimentStarted = false;
    emit experimentStopping();
}

void ExperimentContext::loadFromConfig()
{
    // TODO: values will be saved into config, despite of they just have been read
    setCurrentScenarioIndex(AppConfig::selectedScenarioIndex(_experimentIndex));
    setBatchSize(AppConfig::batchSize(_experimentIndex));
}

void ExperimentContext::recognitionFinished(const ExperimentProbe &p)
{
    _result.accumulate(p);
    emit experimentResultReady();
}
