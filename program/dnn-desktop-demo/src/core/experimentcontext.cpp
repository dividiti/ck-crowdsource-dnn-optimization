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

void ExperimentContext::startExperiment(bool resume) {
    if (!resume) {
        clearAggregatedResults();
    }
    _mode = AppConfig::currentModeType();
    _batchSize = AppConfig::batchSize();
    _isExperimentStarted = true;
    _isExperimentInterrupted = false;
    emit experimentStarted(resume);
}

void ExperimentContext::stopExperiment() {
    _isExperimentStarted = false;
    _isExperimentInterrupted = true;
    emit experimentStopping();
}

void ExperimentContext::notifyModeChanged(const Mode& mode) {
    emit modeChanged(mode);
}

void ExperimentContext::clearAggregatedResults() {
    _duration.clear();
    _precision.clear();
    _top1.clear();
    _top5.clear();
    _lastResult = ImageResult();
}

void ExperimentContext::aggregateResults(ImageResult ir) {
    _duration.add(ir.duration);
    _precision.add(ir.precision());
    _top1.add(ir.correctAsTop1() ? 1 : 0);
    _top5.add(ir.correctAsTop5() ? 1 : 0);
    _lastResult = ir;
}

void ExperimentContext::zoomIn() {
    emit zoomChanged(AppConfig::adjustZoom(true));
}

void ExperimentContext::zoomOut() {
    emit zoomChanged(AppConfig::adjustZoom(false));
}

void ExperimentContext::zoomActual() {
    AppConfig::setZoom(1);
    emit zoomChanged(1);
}
