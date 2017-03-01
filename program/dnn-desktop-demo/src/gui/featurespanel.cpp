#include "appconfig.h"
#include "appmodels.h"
#include "experimentcontext.h"
#include "featurespanel.h"
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

    _infoEngine = makeInfoLabel();
    _infoModel = makeInfoLabel();
    _infoImages = makeInfoLabel();

    _linkSelectEngine = makeLink("Select", "Select another engine", SLOT(selectEngine()));
    _linkSelectModel = makeLink("Select", "Select another scenario", SLOT(selectModel()));
    _linkSelectImages = makeLink("Select", "Select image source", SLOT(selectImages()));

    auto panelEngine = new QFrame;
    panelEngine->setProperty("qss-role", "features-panel");
    panelEngine->setLayout(Ori::Gui::layoutV(0, 0, {
        Ori::Gui::layoutH({ Utils::makeTitle("CAFFE ENGINE"), 0, _linkSelectEngine }),
        _infoEngine,
    }));

    auto panelModel = new QFrame;
    panelModel->setProperty("qss-role", "features-panel");
    panelModel->setLayout(Ori::Gui::layoutV(0, 0, {
        Ori::Gui::layoutH({ Utils::makeTitle("CAFFE MODEL"), 0, _linkSelectModel }),
        _infoModel,
    }));

    auto panelImages = new QFrame;
    panelImages->setProperty("qss-role", "features-panel");
    panelImages->setLayout(Ori::Gui::layoutV(0, 0, {
        Ori::Gui::layoutH({ Utils::makeTitle("IMAGE SOURCE"), 0, _linkSelectImages }),
        _infoImages,
    }));

    setLayout(Ori::Gui::layoutV(0, 0,
    {
        panelEngine,
        Utils::makeDivider(),
        panelModel,
        Utils::makeDivider(),
        panelImages,
    }));
}

QWidget* FeaturesPanel::makeLink(const QString& text, const QString& tooltip, const char* slot)
{
    auto link = new QLabel(QString("<a href='dummy'><span style='color:#969C9E'>%1</span></a>").arg(text));
    link->setAlignment(Qt::AlignTop | Qt::AlignRight);
    link->setProperty("qss-role", "link");
    link->setToolTip(tooltip);
    connect(link, SIGNAL(linkActivated(QString)), this, slot);
    return link;
}

QLabel* FeaturesPanel::makeInfoLabel()
{
    auto label = new QLabel;
    label->setWordWrap(true);
    label->setProperty("qss-role", "info-label");
    return label;
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

    _infoEngine->setText(_context->engines().hasCurrent()
        ? _context->engines().current().title()//.replace("(", "\n(")
        : NA);

    _infoModel->setText(_context->models().hasCurrent()
        ? _context->models().current().title()//.replace("(", "\n(")
        : NA);

    _infoImages->setText(_context->images().hasCurrent()
        ? _context->images().current().title()//.replace("(", "\n(")
        : NA);

//    _infoBatchSize->setInfo(QString::number(_context->batchSize()));
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
//    _linkSetBatchSize->setVisible(on);
}

void FeaturesPanel::experimentStarted()
{
    enableControls(false);
}

void FeaturesPanel::experimentFinished()
{
    enableControls(true);
}
