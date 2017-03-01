#include "experimentcontext.h"
#include "experimentpanel.h"
#include "framespanel.h"
#include "featurespanel.h"
#include "resultspanel.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QPushButton>

#define PANELS_W

ExperimentPanel::ExperimentPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
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

    auto buttonPublish = new QPushButton;
    buttonPublish->setToolTip(tr("Publish"));
    buttonPublish->setIcon(QIcon(":/tools/publish"));
    buttonPublish->setObjectName("buttonPublish");
    buttonPublish->setEnabled(false);

    auto framesPanel = new FramesPanel(context);
    _featuresPanel = new FeaturesPanel(context);
    auto resultsPanel = new ResultsPanel(context);

    auto buttonsPanel = new QFrame;
    buttonsPanel->setObjectName("buttonsPanel");
    buttonsPanel->setLayout(Ori::Gui::layoutH({0, _buttonStart, _buttonStop, buttonPublish, 0}));

    adjustSidebar(_featuresPanel);
    adjustSidebar(resultsPanel);
    adjustSidebar(buttonsPanel);

    setLayout(Ori::Gui::layoutH(0,0,
    {
        framesPanel,
        Ori::Gui::layoutV(0, 0, { _featuresPanel, resultsPanel, buttonsPanel })
    }));
}

void ExperimentPanel::updateExperimentConditions()
{
    _featuresPanel->updateExperimentConditions();
}

void ExperimentPanel::startExperiment()
{
    _context->startExperiment();
}

void ExperimentPanel::stopExperiment()
{
    _buttonStop->setEnabled(false);
    _context->stopExperiment();
}

void ExperimentPanel::experimentStarted()
{
    enableControls(false);
}

void ExperimentPanel::experimentFinished()
{
    enableControls(true);
}

void ExperimentPanel::enableControls(bool on)
{
    _buttonStart->setVisible(on);
    _buttonStart->setEnabled(on);
    _buttonStop->setEnabled(!on);
    _buttonStop->setVisible(!on);
}

void ExperimentPanel::adjustSidebar(QWidget* w)
{
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    w->setFixedWidth(226);
}
