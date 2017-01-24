#include "appconfig.h"
#include "experimentcontext.h"
#include "platformfeaturesprovider.h"
#include "scenariosprovider.h"

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
        AppConfig::setSelectedScenarioIndex(experimentIndex, index);
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
        AppConfig::setBatchSize(experimentIndex, value);
    }
}

void ExperimentContext::startExperiment()
{
    _isExperimentStarted = true;
    emit experimentStarted();
}

void ExperimentContext::stopExperiment()
{
    _isExperimentStarted = false;
    emit experimentStopped();
}

void ExperimentContext::loadFromConfig()
{
    // TODO: values will be saved into config, despite of they just have been read
    setCurrentScenarioIndex(AppConfig::selectedScenarioIndex(experimentIndex));
    setBatchSize(AppConfig::batchSize(experimentIndex));
}
