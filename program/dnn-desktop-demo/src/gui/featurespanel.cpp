#include "appconfig.h"
#include "appmodels.h"
#include "appevents.h"
#include "experimentcontext.h"
#include "featurespanel.h"
#include "engineselectordialog.h"

#include <QBoxLayout>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QScrollArea>

namespace {
QLabel* makeInfoLabel()
{
    auto label = new QLabel;
    label->setWordWrap(true);
    label->setProperty("qss-role", "info-label");
    return label;
}

QWidget* makeDivider()
{
    auto divider = new QFrame;
    divider->setProperty("qss-role", "divider");
    divider->setFrameShape(QFrame::HLine);
    return divider;
}

QFrame* makeFeaturesPanel(const QString& title, QWidget* selectFeatureLink, QWidget* content)
{
    auto panel = new QFrame;
    panel->setProperty("qss-role", "features-panel");

    auto titleLabel = new QLabel(title);
    titleLabel->setProperty("qss-role", "panel-title");
    auto f = titleLabel->font();
    f.setLetterSpacing(QFont::AbsoluteSpacing, 0.8);
    titleLabel->setFont(f);

    auto titleLayout = new QHBoxLayout;
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(selectFeatureLink);

    auto layout = new QVBoxLayout(panel);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(titleLayout);
    layout->addWidget(content);

    return panel;
}
}

FeaturesPanel::FeaturesPanel(ExperimentContext* context, QWidget *parent) : QFrame(parent)
{
    setObjectName("featuresPanel");

    _context = context;
    connect(_context, &ExperimentContext::experimentStarted, this, &FeaturesPanel::experimentStarted);
    connect(_context, &ExperimentContext::experimentFinished, this, &FeaturesPanel::experimentFinished);

    _infoEngine = makeInfoLabel();
    _infoModel = makeInfoLabel();
    _infoImages = makeInfoLabel();
    _infoMode = makeInfoLabel();

    _linkSelectEngine = makeLink("Select", "Select another engine", SLOT(selectEngine()));
    _linkSelectModel = makeLink("Select", "Select another scenario", SLOT(selectModel()));
    _linkSelectImages = makeLink("Select", "Select image source", SLOT(selectImages()));
    _linkSelectMode = makeLink("Select", "Select mode", SLOT(selectMode()));

    auto panelEngine = makeFeaturesPanel("ENGINE", _linkSelectEngine, _infoEngine);
    auto panelModel = makeFeaturesPanel("MODEL", _linkSelectModel, _infoModel);
    auto panelImages = makeFeaturesPanel("IMAGE SOURCE", _linkSelectImages, _infoImages);
    auto panelMode = makeFeaturesPanel("MODE", _linkSelectMode, _infoMode);

    auto settingsPanel = new QFrame;
    auto settingsLayout = new QVBoxLayout(settingsPanel);
    settingsLayout->setMargin(0);
    settingsLayout->setSpacing(0);
    settingsLayout->addWidget(panelEngine);
    settingsLayout->addWidget(makeDivider());
    settingsLayout->addWidget(panelModel);
    settingsLayout->addWidget(makeDivider());
    settingsLayout->addWidget(panelImages);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(panelMode);
    mainLayout->addWidget(makeDivider());
    mainLayout->addWidget(settingsPanel);
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

template<typename T>
static QVariant selectCurrentViaDialog(const QList<T>& items, QVariant current) {
    QDialog dlg;
    auto layout = new QVBoxLayout;
    QVector<QRadioButton*> flags;
    for (int i = 0; i < items.size(); i++) {
        auto flag = new QRadioButton(items.at(i).title());
        flag->setChecked(current.isValid() && items[i] == current.value<T>());
        layout->addWidget(flag);
        flags << flag;
    }
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    dlg.connect(buttons, SIGNAL(accepted()), &dlg, SLOT(accept()));
    dlg.connect(buttons, SIGNAL(rejected()), &dlg, SLOT(reject()));
    layout->addSpacing(12);
    layout->addWidget(buttons);
    dlg.setLayout(layout);
    QVariant ret;
    if (dlg.exec() == QDialog::Accepted) {
        for (int i = 0; i < flags.size(); i++) {
            if (flags.at(i)->isChecked()) {
                ret.setValue(items[i]);
                break;
            }
        }
    }
    return ret;
}

void FeaturesPanel::selectEngine() {
    QVariant v = EngineSelectorDialog::selectEngineAndBatchSize();
    if (v.isValid()) {
        AppConfig::setCurrentProgram(v.value<Program>().targetUoa);
        updateExperimentConditions();
    }
}

void FeaturesPanel::selectModel() {
    auto list = AppConfig::models();
    if (list.isEmpty()) {
        return AppEvents::info("Models not found");
    }
    QVariant v = selectCurrentViaDialog(list, AppConfig::currentModel());
    if (v.isValid()) {
        AppConfig::setCurrentModel(v.value<Model>().uoa);
        updateExperimentConditions();
    }
}

void FeaturesPanel::selectImages() {
    auto list = AppConfig::datasets();
    if (list.isEmpty()) {
        return AppEvents::info("Image datasets not found");
    }
    QVariant v = selectCurrentViaDialog(list, AppConfig::currentDataset());
    if (v.isValid()) {
        AppConfig::setCurrentDataset(v.value<Dataset>().valUoa);
        updateExperimentConditions();
    }
}

void FeaturesPanel::selectMode() {
    auto list = AppConfig::modes();
    if (list.isEmpty()) {
        return AppEvents::info("No run modes found");
    }
    QVariant v = selectCurrentViaDialog(list, AppConfig::currentMode());
    if (v.isValid()) {
        Mode m = v.value<Mode>();
        AppConfig::setCurrentMode(m.type);
        _context->notifyModeChanged(m);
        updateExperimentConditions();
    }
}

void FeaturesPanel::updateExperimentConditions() {
    static const QString NA("N/A");

    auto m = AppConfig::currentModeType();

    QVariant v = AppConfig::currentProgram(m);
    _infoEngine->setText(v.isValid() ? v.value<Program>().title() : NA);

    v = AppConfig::currentModel(m);
    _infoModel->setText(v.isValid() ? v.value<Model>().title() : NA);

    v = AppConfig::currentDataset(m);
    _infoImages->setText(v.isValid() ? v.value<Dataset>().title() : NA);

    _infoMode->setText(Mode(m).title());
}

void FeaturesPanel::enableControls(bool on)
{
    _linkSelectEngine->setVisible(on);
    _linkSelectModel->setVisible(on);
    _linkSelectImages->setVisible(on);
    _linkSelectMode->setVisible(on);
}

void FeaturesPanel::experimentStarted()
{
    enableControls(false);
}

void FeaturesPanel::experimentFinished()
{
    enableControls(true);
}
