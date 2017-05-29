#include "appconfig.h"
#include "experimentcontext.h"

#include <QBoxLayout>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QRadioButton>

ExperimentContext::ExperimentContext() {
    connect(this, &ExperimentContext::newImageResult, this, &ExperimentContext::aggregateResults);
}

void ExperimentContext::startExperiment() {
    clearAggregatedResults();
    _isExperimentStarted = true;
    emit experimentStarted();
}

void ExperimentContext::stopExperiment() {
    _isExperimentStarted = false;
    emit experimentStopping();
}

void ExperimentContext::notifyModeChanged(const Mode& mode) {
    emit modeChanged(mode);
}

void ExperimentContext::clearAggregatedResults() {
    _duration.clear();
    _precision.clear();
}

void ExperimentContext::aggregateResults(ImageResult ir) {
    _duration.add(ir.duration);
    _precision.add(ir.precision());
}
