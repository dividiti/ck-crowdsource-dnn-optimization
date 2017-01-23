#ifndef EXPERIMENTCONTEXT_H
#define EXPERIMENTCONTEXT_H

#include "appmodels.h"

#include <QList>

class PlatformFeaturesProvider;
class ScenariosProvider;

class ExperimentContext
{
public:
    PlatformFeaturesProvider* platformFeaturesProvider;
    ScenariosProvider* scenariosProvider;

    bool checkScenarioIndex(int index) const;

    bool currentScenarioExists() const;
    int currentScenarioIndex() const { return _currentScenarioIndex; }
    void setCurrentScenarioIndex(int index);
    const RecognitionScenario& currentScenario() const;

    const QList<RecognitionScenario>& currentScenarios() const;

private:
    int _currentScenarioIndex = -1;
};

#endif // EXPERIMENTCONTEXT_H
