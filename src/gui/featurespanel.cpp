#include "appmodels.h"
#include "experimentcontext.h"
#include "featurespanel.h"
#include "infolabel.h"
#include "platformfeaturesprovider.h"
#include "scenarioslistwidget.h"
#include "scenariosprovider.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPushButton>

FeaturesPanel::FeaturesPanel(ExperimentContext* context, QWidget *parent) : QWidget(parent)
{
    _context = context;

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    auto buttonStart = new QPushButton(tr("Start"));
    buttonStart->setEnabled(false);

    _infoPlatform = new InfoLabel(tr("Platform:"));
    _infoScenario = new InfoLabel(tr("Scenario:"));
    _infoBatchSize = new InfoLabel(tr("Batch size:"));

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
                makeLink(tr("Info"), tr("Platform info"), SLOT(showPlatformInfo())),
                0
            })
        }),
        Utils::makeDivider(),
        Ori::Gui::layoutV(0, Ori::Gui::layoutSpacing(),
        {
            _infoScenario,
            Ori::Gui::layoutH(
            {
                makeLink(tr("Info"), tr("Show scenario info"), SLOT(showScenarioInfo())),
                makeLink(tr("Select"), tr("Select another scenario"), SLOT(selectScenario())),
                0
            })
        }),
        Utils::makeDivider(),
        _infoBatchSize,
        0,
        buttonStart
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
    auto features = _context->platformFeaturesProvider->current();
    if (features.isEmpty())
        return Utils::infoDlg(tr("Platform features is not collected yet"));

    Utils::showHtmlInfoWindow(features.html(), 600, 800);
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
        _scenariosWindow = new ScenariosListWidget(scenarios, _context->currentScenarioIndex());
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

    auto features = _context->platformFeaturesProvider->current();
    _infoPlatform->setInfo(features.isEmpty()? NA: features.osName());

    _infoScenario->setInfo(_context->currentScenarioExists()
        ? _context->currentScenario().title().replace(": ", "\n")
        : NA);

    _infoBatchSize->setInfo(NA);
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
