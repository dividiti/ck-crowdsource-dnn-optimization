#include "appconfig.h"
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

    _infoEngine = new InfoLabel(tr("Caffe Engine:"));
    _infoModel = new InfoLabel(tr("Caffe Model:"));
    _infoBatchSize = new InfoLabel(tr("Frames count:"));

    _linkSelectEngine = makeLink(tr("Select"), tr("Select another engine"), SLOT(selectEngine()));
    _linkSelectScenario = makeLink(tr("Select"), tr("Select another scenario"), SLOT(selectModel()));
    _linkSetBatchSize = makeLink(tr("Change"), tr("Change batch size"), SLOT(setBatchSize()));

    setLayout(Ori::Gui::layoutV(0, 3*Ori::Gui::layoutSpacing(),
    {
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoEngine,
            Ori::Gui::layoutH({ _linkSelectEngine, 0 })
        }),
        Utils::makeDivider(),
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoModel,
            Ori::Gui::layoutH({ _linkSelectScenario, 0 })
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

void FeaturesPanel::selectEngine()
{
    if (_context->engines().isEmpty())
        return Utils::infoDlg(tr("Recognition engines is not loaded"));

    if (_context->engines().selectCurrentViaDialog())
    {
        AppConfig::setSelectedEngineIndex(_context->experimentIndex(), _context->engines().currentIndex());
        updateExperimentConditions();
    }
}

void FeaturesPanel::selectModel()
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

    _infoEngine->setInfo(_context->engines().hasCurrent()
        ? _context->engines().current().title().replace("(", "\n(")
        : NA);

    _infoModel->setInfo(_context->currentScenarioExists()
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
    _linkSelectEngine->setVisible(on);
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
