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
    if (checkScenarioIndex(index)) _currentScenarioIndex = index;
}

const RecognitionScenario& ExperimentContext::currentScenario() const
{
    return currentScenarios().at(_currentScenarioIndex);
}

const QList<RecognitionScenario>& ExperimentContext::currentScenarios() const
{
    return scenariosProvider->currentList().items();
}
