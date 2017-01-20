#include "appmodels.h"
#include "experimentcontext.h"
#include "featurespanel.h"
#include "infolabel.h"
#include "platformfeaturesprovider.h"
#include "scenarioslistwidget.h"
#include "scenariosprovider.h"
#include "utils.h"
#include "../ori/OriWidgets.h"
#include "../ori/OriLabels.h"

#include <QBoxLayout>
#include <QPushButton>

FeaturesPanel::FeaturesPanel(ExperimentContext* context, QWidget *parent) : QWidget(parent)
{
    _context = context;

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    auto buttonStart = new QPushButton(tr("Start"));
    buttonStart->setEnabled(false);

    _infoPlatform = new InfoLabel(tr("Platform:"));
    _infoScenario = new InfoLabel(tr("Engine:"));
    _infoBatchSize = new InfoLabel(tr("Batch size:"));
    _infoScenario->setInfo("AlexNet / CPU");
    _infoBatchSize->setInfo("2");

/*
    auto buttonPlatform = new Ori::Widgets::ClickableLabel;
    buttonPlatform->setPixmap(QPixmap(":/tools/info"));
    connect(buttonPlatform, SIGNAL(clicked()), this, SLOT(showPlatformInfo()));

    auto buttonScenario1 = new Ori::Widgets::ClickableLabel;
    buttonScenario1->setPixmap(QPixmap(":/tools/info"));
    connect(buttonScenario1, SIGNAL(clicked()), this, SLOT(showScenarioInfo()));

    auto buttonScenario2 = new Ori::Widgets::ClickableLabel;
    buttonScenario2->setPixmap(QPixmap(":/tools/info"));
    connect(buttonScenario2, SIGNAL(clicked()), this, SLOT(selectScenario()));

    auto layoutPlatformButton = new QHBoxLayout;
    layoutPlatformButton->setMargin(0);
    layoutPlatformButton->setSpacing(0);
    layoutPlatformButton->addWidget(buttonPlatform);
    layoutPlatformButton->addStretch();

    auto layoutPlatform = new QVBoxLayout;
    layoutPlatform->setMargin(0);
    layoutPlatform->setSpacing(0);
    layoutPlatform->addWidget(infoPlatform);
    layoutPlatform->addLayout(layoutPlatformButton);

    auto layoutScenarioButton = new QHBoxLayout;
    layoutScenarioButton->setMargin(0);
    layoutScenarioButton->setSpacing(0);
    layoutScenarioButton->addWidget(buttonScenario1);
    layoutScenarioButton->addWidget(buttonScenario2);
    layoutScenarioButton->addStretch();

    auto layoutScenario = new QVBoxLayout;
    layoutScenario->setMargin(0);
    layoutScenario->setSpacing(0);
    layoutScenario->addWidget(infoScenario);
    layoutScenario->addLayout(layoutScenarioButton);

    auto layout = new QVBoxLayout;
    layout->addLayout(layoutPlatform);
    layout->addLayout(layoutScenario);
    layout->addWidget(infoBatchSize);
    layout->addStretch();
    layout->addWidget(buttonStart);
    setLayout(layout);
*/
    setLayout(Ori::Gui::layoutV(0, 3*Ori::Gui::layoutSpacing(),
    {
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoPlatform,
            Ori::Gui::layoutH(
            {
                makeToolIcon(tr("Platform info"), ":/tools/info", SLOT(showPlatformInfo())),
                0
            })
        }),
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoScenario,
            Ori::Gui::layoutH(
            {
                makeToolIcon(tr("Scenario info"), ":/tools/info", SLOT(showScenarioInfo())),
                makeToolIcon(tr("Select scenario"), ":/tools/ok", SLOT(selectScenario())),
                0
            })
        }),
        _infoBatchSize,
        0,
        buttonStart
    }));
}

QWidget* FeaturesPanel::makeToolIcon(const QString& tooltip, const char* icon, const char* slot)
{
    auto w = new Ori::Widgets::ClickableLabel;
    w->setPixmap(QPixmap(icon));
    w->setToolTip(tooltip);
    connect(w, SIGNAL(clicked()), this, slot);
    return w;
}

void FeaturesPanel::showPlatformInfo()
{
    auto features = _context->platformFeaturesProvider->current();
    if (features.isEmpty())
        return Utils::infoDlg(tr("Platform features is not collected yet"));

    Utils::showTextInfoWindow(features.str(), 600, 800);
}

void FeaturesPanel::showScenarioInfo()
{
    if (_context->currentScenario.isEmpty())
        return Utils::infoDlg(tr("Scenario is not selected yet"));

    Utils::showTextInfoWindow(_context->currentScenario.str(), 600, 800);
}

void FeaturesPanel::selectScenario()
{
    const RecognitionScenarios& scenarios = _context->scenariosProvider->currentList();
    if (scenarios.isEmpty())
        return Utils::infoDlg(tr("Recognition scenarios is not loaded yet"));

    auto w = new ScenariosListWidget(scenarios);
    Utils::moveToDesktopCenter(w);
    w->resize(500, 500);
    w->show();
}

void FeaturesPanel::updateExperimentConditions()
{
    auto features = _context->platformFeaturesProvider->current();
    _infoPlatform->setInfo(features.isEmpty()? "N/A": features.osName());
}
