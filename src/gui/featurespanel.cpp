#include "appmodels.h"
#include "experimentcontext.h"
#include "featurespanel.h"
#include "infolabel.h"
#include "scenarioslistwidget.h"
#include "scenariosprovider.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QPushButton>

FeaturesPanel::FeaturesPanel(ExperimentContext* context, QWidget *parent) : QWidget(parent)
{
    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentFinished()), this, SLOT(experimentFinished()));

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _buttonStart = new QPushButton(tr("Start"));
    _buttonStop = new QPushButton(tr("Stop"));
    _buttonStop->setVisible(false);
    connect(_buttonStart, SIGNAL(pressed()), this, SLOT(startExperiment()));
    connect(_buttonStop, SIGNAL(pressed()), this, SLOT(stopExperiment()));

    _infoPlatform = new InfoLabel(tr("Platform:"));
    _infoScenario = new InfoLabel(tr("Caffe model:"));
    _infoBatchSize = new InfoLabel(tr("Frames count:"));

    auto linkShowPlatformInfo = makeLink(tr("Info"), tr("Platform info"), SLOT(showPlatformInfo()));
    auto linkShowScenarioInfo = makeLink(tr("Info"), tr("Show scenario info"), SLOT(showScenarioInfo()));
    _linkSelectScenario = makeLink(tr("Select"), tr("Select another scenario"), SLOT(selectScenario()));
    _linkSetBatchSize = makeLink(tr("Change"), tr("Change batch size"), SLOT(setBatchSize()));

    setLayout(Ori::Gui::layoutV(0, 3*Ori::Gui::layoutSpacing(),
    {
        /*Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoPlatform,
            Ori::Gui::layoutH({ linkShowPlatformInfo, 0 })
        }),
        Utils::makeDivider(),*/
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoScenario,
            Ori::Gui::layoutH({ linkShowScenarioInfo, _linkSelectScenario, 0 })
        }),
        Utils::makeDivider(),
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoBatchSize,
            Ori::Gui::layoutH({ _linkSetBatchSize, 0 })
        }),
        0,
        _buttonStart,
        _buttonStop
    }));
}

FeaturesPanel::~FeaturesPanel()
{
    if (_scenariosWindow)
        delete _scenariosWindow;
}

QWidget* FeaturesPanel::makeLink(const QString& text, const QString& tooltip, const char* slot)
{
    auto link = new QLabel(QString("<a href='dummy'>%1</a>").arg(text));
    link->setToolTip(tooltip);
    connect(link, SIGNAL(linkActivated(QString)), this, slot);
    return link;
}

void FeaturesPanel::showPlatformInfo()
{
//    auto features = _context->platformFeaturesProvider->current();
//    if (features.isEmpty())
//        return Utils::infoDlg(tr("Platform features is not collected yet"));

//    Utils::showHtmlInfoWindow(features.html(), 600, 800);
}

void FeaturesPanel::showScenarioInfo()
{
    if (!_context->currentScenarioExists() )
        return Utils::infoDlg(tr("Scenario is not selected yet"));

    Utils::showHtmlInfoWindow(_context->currentScenario().html(), 600, 800);
}

void FeaturesPanel::selectScenario()
{
    const RecognitionScenarios& scenarios = _context->scenariosProvider->currentList();
    if (scenarios.isEmpty())
        return Utils::infoDlg(tr("Recognition scenarios is not loaded yet"));

    if (!_scenariosWindow)
    {
        _scenariosWindow = new ScenariosListWidget(_context);
        connect(_scenariosWindow.data(), SIGNAL(currentScenarioSelected(int)), this, SLOT(currentScenarioSelected(int)));
        Utils::moveToDesktopCenter(_scenariosWindow);
        _scenariosWindow->show();
    }
    else
        _scenariosWindow->activateWindow();
}

void FeaturesPanel::updateExperimentConditions()
{
    static QString NA("N/A");

    //auto features = _context->platformFeaturesProvider->current();
    //_infoPlatform->setInfo(features.isEmpty()? NA: features.osName());

    _infoScenario->setInfo(_context->currentScenarioExists()
        ? _context->currentScenario().title().replace("(", "\n(")
        : NA);

    _infoBatchSize->setInfo(QString::number(_context->batchSize()));
}

void FeaturesPanel::currentScenarioSelected(int index)
{
    qDebug() << "currentScenarioSelected" << index;
    if (_context->currentScenarioIndex() != index)
    {
        _context->setCurrentScenarioIndex(index);
        updateExperimentConditions();
    }
}

void FeaturesPanel::setBatchSize()
{
    int batchSize = QInputDialog::getInt(this, tr("Batch Size"), tr("Set batch size:"),
        _context->batchSize(), _context->minBatchSize(), _context->maxBatchSize());
    if (batchSize != _context->batchSize())
    {
        _context->setBatchSize(batchSize);
        updateExperimentConditions();
    }
}

void FeaturesPanel::enableControls(bool on)
{
    _buttonStart->setVisible(on);
    _buttonStart->setEnabled(on);
    _buttonStop->setEnabled(!on);
    _buttonStop->setVisible(!on);
    _linkSelectScenario->setVisible(on);
    _linkSetBatchSize->setVisible(on);
}

void FeaturesPanel::startExperiment()
{
    _context->startExperiment();
}

void FeaturesPanel::stopExperiment()
{
    _buttonStop->setEnabled(false);
    _context->stopExperiment();
}

void FeaturesPanel::experimentStarted()
{
    enableControls(false);
}

void FeaturesPanel::experimentFinished()
{
    enableControls(true);
}
