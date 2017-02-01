#include "appconfig.h"
#include "appmodels.h"
#include "experimentcontext.h"
#include "featurespanel.h"
#include "infolabel.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>

FeaturesPanel::FeaturesPanel(ExperimentContext* context, QWidget *parent) : QFrame(parent)
{
    setObjectName("featuresPanel");

    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentFinished()), this, SLOT(experimentFinished()));

    _infoEngine = new InfoLabel("CAFFE ENGINE");
    _infoModel = new InfoLabel("CAFFE MODEL");
    _infoImages = new InfoLabel("IMAGE SOURCE");
    _infoBatchSize = new InfoLabel("FRAMES COUNT");

    _linkSelectEngine = makeLink("Select", "Select another engine", SLOT(selectEngine()));
    _linkSelectModel = makeLink("Select", "Select another scenario", SLOT(selectModel()));
    _linkSelectImages = makeLink("Select", "Select image source", SLOT(selectImages()));
    _linkSetBatchSize = makeLink("Change", "Change batch size", SLOT(setBatchSize()));

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
            Ori::Gui::layoutH({ _linkSelectModel, 0 })
        }),
        Utils::makeDivider(),
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoImages,
            Ori::Gui::layoutH({ _linkSelectImages, 0 })
        }),
        Utils::makeDivider(),
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoBatchSize,
            Ori::Gui::layoutH({ _linkSetBatchSize, 0 })
        }),
        0,
    }));
}

QWidget* FeaturesPanel::makeLink(const QString& text, const QString& tooltip, const char* slot)
{
    auto link = new QLabel(QString("<a href='dummy'><span style='color:#969C9E'>%1</span></a>").arg(text));
    link->setProperty("qss-role", "link");
    link->setToolTip(tooltip);
    connect(link, SIGNAL(linkActivated(QString)), this, slot);
    return link;
}

void FeaturesPanel::selectEngine()
{
    if (_context->engines().isEmpty())
        return Utils::infoDlg(tr("Recognition engines not found"));

    if (_context->engines().selectCurrentViaDialog())
    {
        AppConfig::setSelectedEngineIndex(_context->experimentIndex(), _context->engines().currentIndex());
        updateExperimentConditions();
    }
}

void FeaturesPanel::selectModel()
{
    if (_context->models().isEmpty())
        return Utils::infoDlg(tr("Recognition models not found"));

    if (_context->models().selectCurrentViaDialog())
    {
        AppConfig::setSelectedModelIndex(_context->experimentIndex(), _context->models().currentIndex());
        updateExperimentConditions();
    }
}

void FeaturesPanel::selectImages()
{
    if (_context->images().isEmpty())
        return Utils::infoDlg(tr("Image datasets not found"));

    if (_context->images().selectCurrentViaDialog())
    {
        AppConfig::setSelectedImagesIndex(_context->experimentIndex(), _context->images().currentIndex());
        updateExperimentConditions();
    }
}

void FeaturesPanel::updateExperimentConditions()
{
    static QString NA("N/A");

    _infoEngine->setInfo(_context->engines().hasCurrent()
        ? _context->engines().current().title().replace("(", "\n(")
        : NA);

    _infoModel->setInfo(_context->models().hasCurrent()
        ? _context->models().current().title().replace("(", "\n(")
        : NA);

    _infoImages->setInfo(_context->images().hasCurrent()
        ? _context->images().current().title().replace("(", "\n(")
        : NA);

    _infoBatchSize->setInfo(QString::number(_context->batchSize()));
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
    _linkSelectEngine->setVisible(on);
    _linkSelectModel->setVisible(on);
    _linkSelectImages->setVisible(on);
    _linkSetBatchSize->setVisible(on);
}

void FeaturesPanel::experimentStarted()
{
    enableControls(false);
}

void FeaturesPanel::experimentFinished()
{
    enableControls(true);
}
