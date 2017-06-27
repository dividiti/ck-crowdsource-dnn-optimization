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
    _results = QVector<ImageResult>();
    _current_result = -1;
}

void ExperimentContext::aggregateResults(ImageResult ir) {
    _duration.add(ir.duration);
    _precision.add(ir.precision());
    _top1.add(ir.correctAsTop1() ? 1 : 0);
    _top5.add(ir.correctAsTop5() ? 1 : 0);
    _results.append(ir);
    if (_current_result == _results.size() - 2) {
        _current_result = _results.size() - 1;
    }
    emitCurrentResult();
}

void ExperimentContext::emitZoomChanged(double z, bool ztf) {
    emit zoomChanged(z, ztf);
}

void ExperimentContext::zoomIn() {
    emitZoomChanged(AppConfig::adjustZoom(true), AppConfig::zoomToFit());
}

void ExperimentContext::zoomOut() {
    emitZoomChanged(AppConfig::adjustZoom(false), AppConfig::zoomToFit());
}

void ExperimentContext::zoomActual() {
    emitZoomChanged(AppConfig::setZoom(1), AppConfig::zoomToFit());
}

void ExperimentContext::zoomToFit() {
    emitZoomChanged(AppConfig::zoom(), AppConfig::toggleZoomToFit());
}

void ExperimentContext::emitCurrentResult() {
    emit currentResultChanged(_current_result, _results.size(), _results.at(_current_result));
}

void ExperimentContext::gotoNextResult() {
    if (_current_result < _results.size() - 1) {
        ++_current_result;
        emitCurrentResult();
    }
}

void ExperimentContext::gotoPrevResult() {
    if (0 < _current_result && _current_result < _results.size()) {
        --_current_result;
        emitCurrentResult();
    }
}

void ExperimentContext::gotoFirstResult() {
    if (!_results.empty() && 0 < _current_result) {
        _current_result = 0;
        emitCurrentResult();
    }
}

void ExperimentContext::gotoLastResult() {
    if (!_results.empty() && _current_result < _results.size() - 1) {
        _current_result = _results.size() - 1;
        emitCurrentResult();
    }
}
