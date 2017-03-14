#include "appconfig.h"
#include "experimentcontext.h"

#include <QBoxLayout>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QRadioButton>

void ExperimentContext::startExperiment()
{
    _isExperimentStarted = true;
    emit experimentStarted();
}

void ExperimentContext::stopExperiment()
{
    _isExperimentStarted = false;
    emit experimentStopping();
}

