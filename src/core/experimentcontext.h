#ifndef EXPERIMENTCONTEXT_H
#define EXPERIMENTCONTEXT_H

#include "appmodels.h"

class PlatformFeaturesProvider;
class ScenariosProvider;

class ExperimentContext
{
public:
    ExperimentContext();

    PlatformFeaturesProvider* platformFeaturesProvider;
    ScenariosProvider* scenariosProvider;

    RecognitionScenario currentScenario;
};

#endif // EXPERIMENTCONTEXT_H
