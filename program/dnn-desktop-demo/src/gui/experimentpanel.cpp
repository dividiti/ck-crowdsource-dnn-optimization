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
#include <QVariant>
#include <QDebug>

#define PANELS_W

static const long KILL_WAIT_MS = 1000 * 60;

ExperimentPanel::ExperimentPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent) {
    setObjectName("experimentPanel");

    _context = context;
    connect(_context, &ExperimentContext::experimentStarted, this, &ExperimentPanel::experimentStarted);
    connect(_context, &ExperimentContext::experimentFinished, this, &ExperimentPanel::experimentFinished);
    connect(_context, &ExperimentContext::modeChanged, this, &ExperimentPanel::modeChanged);
    connect(_context, &ExperimentContext::publishStarted, this, &ExperimentPanel::publishStarted);
    connect(_context, &ExperimentContext::publishFinished, this, &ExperimentPanel::publishFinished);

    _buttonStart = new QPushButton(tr("Start"));
    _buttonStart->setObjectName("buttonStart");
    _buttonStop = new QPushButton(tr("Pause"));
    _buttonStop->setObjectName("buttonStop");
    _buttonStop->setVisible(false);
    connect(_buttonStart, SIGNAL(clicked(bool)), this, SLOT(startExperiment()));
    connect(_buttonStop, SIGNAL(clicked(bool)), this, SLOT(stopExperiment()));

    _buttonStartOver = new QPushButton(tr("Stop"));
    _buttonStartOver->setObjectName("buttonStartOver");
    _buttonStartOver->setToolTip(tr("Start over"));
    _buttonStartOver->setVisible(false);
    connect(_buttonStartOver, SIGNAL(clicked(bool)), this, SLOT(startOver()));

    auto framesPanel = new FramesPanel(context);
    _featuresPanel = new FeaturesPanel(context);
    auto resultsPanel = new ResultsPanel(context);

    auto buttonsPanel = new QFrame;
    buttonsPanel->setObjectName("buttonsPanel");
    buttonsPanel->setLayout(Ori::Gui::layoutH(0, 8, {_buttonStart, _buttonStop, _buttonStartOver }));

    adjustSidebar(_featuresPanel);
    adjustSidebar(resultsPanel);
    adjustSidebar(buttonsPanel);

    setLayout(Ori::Gui::layoutH(0, 0,
    {
        framesPanel,
        Ori::Gui::layoutV(0, 0, { _featuresPanel, resultsPanel, buttonsPanel })
    }));
}

void ExperimentPanel::updateExperimentConditions() {
    _featuresPanel->updateExperimentConditions();
}

bool ExperimentPanel::canResume() const {
    return _context->resumable() && AppConfig::currentModeType() == _context->mode();
}

void ExperimentPanel::startExperiment() {
    _context->startExperiment(!_restart && canResume());
    _restart = false;
}

void ExperimentPanel::startOver() {
    _restart = true;
    enableControls(true);
}

void ExperimentPanel::stopExperiment() {
    _buttonStop->setEnabled(false);
    _context->stopExperiment();
}

void ExperimentPanel::experimentStarted() {
    enableControls(false);
}

void ExperimentPanel::experimentFinished() {
    enableControls(true);
}

void ExperimentPanel::modeChanged(Mode) {
    enableControls(true);
}

void ExperimentPanel::enableControls(bool on) {
    if (on && !_restart && canResume()) {
        _buttonStart->setText(tr("Resume"));
        _buttonStartOver->setVisible(true);
        _buttonStartOver->setEnabled(true);
    } else {
        _buttonStart->setText(tr("Start"));
        _buttonStartOver->setVisible(false);
    }
    _buttonStart->setVisible(on);
    _buttonStart->setEnabled(on);
    _buttonStop->setEnabled(!on);
    _buttonStop->setVisible(!on);
}

void ExperimentPanel::adjustSidebar(QWidget* w) {
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    w->setFixedWidth(226);
}

void ExperimentPanel::publishStarted() {
    _buttonStart->setEnabled(false);
    _buttonStartOver->setEnabled(false);
}

void ExperimentPanel::publishFinished(bool) {
    _buttonStart->setEnabled(true);
    _buttonStartOver->setEnabled(true);
}
