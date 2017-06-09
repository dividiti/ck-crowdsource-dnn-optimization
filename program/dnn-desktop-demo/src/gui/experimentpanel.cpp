#include "experimentcontext.h"
#include "experimentpanel.h"
#include "framespanel.h"
#include "featurespanel.h"
#include "resultspanel.h"
#include "../ori/OriWidgets.h"
#include "../core/appmodels.h"
#include "../core/appconfig.h"
#include "../core/appevents.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QDateTime>
#include <QVariant>
#include <QProcess>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
#include <QDir>
#include <QTemporaryFile>

#define PANELS_W

static const long KILL_WAIT_MS = 1000 * 60;

ExperimentPanel::ExperimentPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent) {
    setObjectName("experimentPanel");

    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentFinished()), this, SLOT(experimentFinished()));

    _buttonStart = new QPushButton(tr("Start"));
    _buttonStart->setObjectName("buttonStart");
    _buttonStop = new QPushButton(tr("Stop"));
    _buttonStop->setObjectName("buttonStop");
    _buttonStop->setVisible(false);
    connect(_buttonStart, SIGNAL(clicked(bool)), this, SLOT(startExperiment()));
    connect(_buttonStop, SIGNAL(clicked(bool)), this, SLOT(stopExperiment()));

    _buttonPublish = new QPushButton;
    _buttonPublish->setToolTip(tr("No results for publishing. Please, start and finish an experiment first."));
    _buttonPublish->setIcon(QIcon(":/tools/publish"));
    _buttonPublish->setObjectName("buttonPublish");
    _buttonPublish->setEnabled(false);
    connect(_buttonPublish, SIGNAL(clicked(bool)), this, SLOT(publishResults()));

    auto framesPanel = new FramesPanel(context);
    _featuresPanel = new FeaturesPanel(context);
    auto resultsPanel = new ResultsPanel(context);

    auto buttonsPanel = new QFrame;
    buttonsPanel->setObjectName("buttonsPanel");
    buttonsPanel->setLayout(Ori::Gui::layoutH({0, _buttonStart, _buttonStop, _buttonPublish, 0}));

    adjustSidebar(_featuresPanel);
    adjustSidebar(resultsPanel);
    adjustSidebar(buttonsPanel);

    setLayout(Ori::Gui::layoutH(0,0,
    {
        framesPanel,
        Ori::Gui::layoutV(0, 0, { _featuresPanel, resultsPanel, buttonsPanel })
    }));

    _publisher = new QProcess;
    _publisher->setWorkingDirectory(AppConfig::ckBinPath());
    _publisher->setProgram(AppConfig::ckExeName());
    connect(_publisher, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(publishResultsFinished(int, QProcess::ExitStatus)));
    connect(_publisher, SIGNAL(error(QProcess::ProcessError)), this, SLOT(publishResultsError(QProcess::ProcessError)));
}

void ExperimentPanel::updateExperimentConditions() {
    _featuresPanel->updateExperimentConditions();
}

void ExperimentPanel::startExperiment() {
    _context->startExperiment();
}

void ExperimentPanel::stopExperiment() {
    _buttonStop->setEnabled(false);
    _buttonStop->setText(tr("Stopping..."));
    _context->stopExperiment();
}

void ExperimentPanel::experimentStarted() {
    enableControls(false);
    _experiment_start_time = QDateTime::currentMSecsSinceEpoch();
    _program = AppConfig::currentProgram().value<Program>();
    _model = AppConfig::currentModel().value<Model>();
    _dataset = AppConfig::currentDataset().value<Dataset>();
}

void ExperimentPanel::experimentFinished() {
    _buttonStop->setText(tr("Stop"));
    enableControls(true);
}

void ExperimentPanel::enableControls(bool on) {
    _buttonStart->setVisible(on);
    _buttonStart->setEnabled(on);
    _buttonStop->setEnabled(!on);
    _buttonStop->setVisible(!on);

    bool pubOn = on && _context->hasAggregatedResults();
    _buttonPublish->setEnabled(pubOn);
    if (pubOn) {
        _buttonPublish->setToolTip(tr("Publish"));
    } else {
        _buttonPublish->setToolTip(tr("Experiment in progress"));
    }
}

void ExperimentPanel::adjustSidebar(QWidget* w) {
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    w->setFixedWidth(226);
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

void ExperimentPanel::publishResults() {
    if (!_context->hasAggregatedResults()) {
        // nothing to publish
        return;
    }
    QJsonObject dict;
    dict["duration"] = toJson(_context->duration());
    dict["detection"] = toJsonDetection(_context);
    dict["classification"] = toJsonClassification(_context);
    dict["mode"] = Mode(_context->mode()).name();
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
    _buttonStart->setEnabled(false);
    _buttonStart->setText(tr("Publishing..."));
    _buttonPublish->setEnabled(false);
    _buttonPublish->setToolTip(tr("Publishing in process"));
}

static void reportPublisherFail(QProcess* publisher) {
    const QString runCmd = publisher->program() + " " +  publisher->arguments().join(" ");
    AppEvents::error("Failed to publish results. "
                     "Please, select the command below, copy it and run manually from command line "
                     "to investigate the issue:\n\n" + runCmd);
}

void ExperimentPanel::publishResultsFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "Publishing results finished with exit code " << exitCode << " and exit status " << exitStatus;
    _buttonStart->setEnabled(true);
    _buttonStart->setText(tr("Start"));
    if (0 != exitCode) {
        reportPublisherFail(_publisher);
        _buttonPublish->setEnabled(true);
        _buttonPublish->setToolTip(tr("Publish"));
    } else {
        AppEvents::info("Results are successfully pushed to the server. Thank you for contributing!");
        _context->clearAggregatedResults();
        _buttonPublish->setEnabled(false);
        _buttonPublish->setToolTip(tr("Already published"));
    }
}

void ExperimentPanel::publishResultsError(QProcess::ProcessError error) {
    qDebug() << "Publishing results error " << error;
    reportPublisherFail(_publisher);
    if (_context->hasAggregatedResults()) {
        _buttonPublish->setEnabled(true);
        _buttonPublish->setToolTip(tr("Publish"));
    }
}
