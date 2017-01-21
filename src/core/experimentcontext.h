#ifndef EXPERIMENTCONTEXT_H
#define EXPERIMENTCONTEXT_H

#include "appmodels.h"

class PlatformFeaturesProvider;
class ScenariosProvider;

class ExperimentContext
{
public:
    PlatformFeaturesProvider* platformFeaturesProvider;
    ScenariosProvider* scenariosProvider;

    bool currentScenarioExists() const;
    int currentScenarioIndex() const { return _currentScenarioIndex; }
    void setCurrentScenarioIndex(int index);
    const RecognitionScenario& currentScenario() const;

private:
    int _currentScenarioIndex = -1;

    bool checkScenarioIndex(int index) const;
};

#endif // EXPERIMENTCONTEXT_H
