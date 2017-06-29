#include "appconfig.h"
#include "appevents.h"
#include "experimentcontext.h"

#include <QBoxLayout>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
#include <QDir>
#include <QTemporaryFile>

ExperimentContext::ExperimentContext() {
    connect(this, &ExperimentContext::newImageResult, this, &ExperimentContext::aggregateResults);
    connect(this, &ExperimentContext::experimentFinished, this, &ExperimentContext::onExperimentFinished);

    _publisher = new QProcess;
    _publisher->setWorkingDirectory(AppConfig::ckBinPath());
    _publisher->setProgram(AppConfig::ckExeName());
    connect(_publisher, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(publishResultsFinished(int, QProcess::ExitStatus)));
    connect(_publisher, SIGNAL(error(QProcess::ProcessError)), this, SLOT(publishResultsError(QProcess::ProcessError)));
}

void ExperimentContext::startExperiment(bool resume) {
    if (!resume) {
        clearAggregatedResults();
    }
    _mode = AppConfig::currentModeType();
    _batchSize = AppConfig::batchSize();
    _program = AppConfig::currentProgram().value<Program>();
    _model = AppConfig::currentModel().value<Model>();
    _dataset = AppConfig::currentDataset().value<Dataset>();
    _isExperimentStarted = true;
    _isExperimentInterrupted = false;
    emit experimentStarted(resume);
}

void ExperimentContext::stopExperiment() {
    _isExperimentStarted = false;
    _isExperimentInterrupted = true;
    emit experimentStopping();
}

void ExperimentContext::onExperimentFinished() {
    _isExperimentStarted = false;
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

static QJsonObject toJson(const ExperimentContext::Stat& stat) {
    QJsonObject dict;
    dict["avg"] = stat.avg;
    dict["min"] = stat.min;
    dict["max"] = stat.max;
    return dict;
}

static QJsonObject toJsonClassification(const ExperimentContext* context) {
    QJsonObject dict;
    dict["top1"] = context->top1().avg;
    dict["top5"] = context->top5().avg;
    dict["batch_size"] = context->batchSize();
    return dict;
}

static QJsonObject toJsonDetection(const ExperimentContext* context) {
    QJsonObject dict;
    dict["precision"] = toJson(context->precision());
    return dict;
}

void ExperimentContext::publishResults() {
    if (!hasAggregatedResults()) {
        // nothing to publish
        return;
    }
    QJsonObject dict;
    dict["duration"] = toJson(duration());
    dict["detection"] = toJsonDetection(this);
    dict["classification"] = toJsonClassification(this);
    dict["mode"] = Mode(mode()).name();
    dict["model_uoa"] = _model.uoa;
    dict["dataset_uoa"] = _dataset.valUoa;
    dict["program_uoa"] = _program.programUoa;
    dict["tmp_dir"] = _program.targetDir;
    dict["engine_uoa"] = _program.targetUoa;
    QJsonObject results;
    results["dict"] = dict;
    QFileInfo out(_program.outputFile);
    QDir dir = out.absoluteDir();
    QTemporaryFile tmp(dir.absolutePath() + QDir::separator() + "tmp-publish-XXXXXX.json");
    tmp.setAutoRemove(false);
    if (!tmp.open()) {
        AppEvents::error("Failed to create a results JSON file");
        return;
    }
    QFileInfo tmpInfo(tmp);
    QString absoluteTmpPath = tmpInfo.absoluteFilePath();
    QJsonDocument doc(results);
    tmp.write(doc.toJson());
    tmp.close();
    _publisher->setArguments(QStringList {
                        "submit",
                        "experiment.bench.dnn.desktop",
                        "@" + absoluteTmpPath
                        });
    const QString runCmd = _publisher->program() + " " +  _publisher->arguments().join(" ");
    qDebug() << "Run CK command: " << runCmd;
    _publisher->start();
    emit publishStarted();
}

static void reportPublisherFail(QProcess* publisher) {
    const QString runCmd = publisher->program() + " " +  publisher->arguments().join(" ");
    AppEvents::error("Failed to publish results. "
                     "Please, select the command below, copy it and run manually from command line "
                     "to investigate the issue:\n\n" + runCmd);
}

void ExperimentContext::publishResultsFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "Publishing results finished with exit code " << exitCode << " and exit status " << exitStatus;
    if (0 != exitCode) {
        reportPublisherFail(_publisher);
        emit publishFinished(false);
    } else {
        AppEvents::info("Results are successfully pushed to the server. Thank you for contributing!");
        emit publishFinished(true);
    }
}

void ExperimentContext::publishResultsError(QProcess::ProcessError error) {
    qDebug() << "Publishing results error " << error;
    reportPublisherFail(_publisher);
    emit publishFinished(false);
}
