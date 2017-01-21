#include "appmodels.h"
#include "scenarioslistwidget.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QRadioButton>
#include <QScrollArea>
#include <QToolBar>

ScenarioItemWidget::ScenarioItemWidget(const RecognitionScenario* scenario, QWidget *parent) : QWidget(parent)
{
    _scenario = scenario;

    auto labelTitle = new QLabel(scenario->title());
    auto f = labelTitle->font();
    f.setBold(true);
    f.setPointSize(f.pointSize()+1);
    labelTitle->setFont(f);

    auto labelSize = new QLabel(scenario->fileSizeMB());

    _selectionFlag = new QRadioButton;
    connect(_selectionFlag, SIGNAL(clicked(bool)), this, SIGNAL(selectionFlagClicked()));

    auto actionDeleteFiles = Ori::Gui::action(tr("Delete scenario files"), this, SLOT(deleteScenarioFiles()), ":/tools/delete");
    auto actionDloadFiles = Ori::Gui::action(tr("Download scenario files"), this, SLOT(downloadsScenarioFiles()), ":/tools/download");
    auto actionShowInfo = Ori::Gui::action(tr("Show scenario info"), this, SLOT(showScenarioInfo()), ":/tools/info");

    setLayout(Ori::Gui::layoutH(
    {
        _selectionFlag,
        Ori::Gui::spacing(16),
        Ori::Gui::layoutV({ labelTitle, labelSize }),
        0,
        Ori::Gui::spacing(64),
        Ori::Gui::toolbar({ actionDeleteFiles, actionDloadFiles, actionShowInfo })
    }));

    bool filesLoaded = scenario->allFilesAreLoaded();
    actionDeleteFiles->setVisible(filesLoaded);
    actionDloadFiles->setVisible(!filesLoaded);
}

void ScenarioItemWidget::deleteScenarioFiles()
{
    Utils::infoDlg("TODO deleteScenarioFiles");
}

void ScenarioItemWidget::downloadsScenarioFiles()
{
    Utils::infoDlg("TODO downloadsScenarioFiles");
}

void ScenarioItemWidget::showScenarioInfo()
{
    Utils::showHtmlInfoWindow(_scenario->html(), 600, 800);
}

void ScenarioItemWidget::useThisScenario()
{
    Utils::infoDlg("TODO useThisScenario");
}

void ScenarioItemWidget::showCurrentScenario()
{
    Utils::infoDlg("TODO showCurrentScenario");
}

void ScenarioItemWidget::setSelected(bool on)
{
    _selectionFlag->setChecked(on);
}

bool ScenarioItemWidget::selected() const
{
    return _selectionFlag->isChecked();
}

//-----------------------------------------------------------------------------

ScenariosListWidget::ScenariosListWidget(const RecognitionScenarios& scenarios, int currentScenario, QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    auto scenariosLayout = new QVBoxLayout;
    for (int i = 0; i < scenarios.items().size(); i++)
    {
        const RecognitionScenario& scenario = scenarios.items().at(i);

        auto scenarioWidget = new ScenarioItemWidget(&scenario);
        scenarioWidget->setSelected(i == currentScenario);
        connect(scenarioWidget, SIGNAL(selectionFlagClicked()), this, SLOT(scenarioSelected()));

        scenariosLayout->addWidget(scenarioWidget);
        _scenarioWidgets.append(scenarioWidget);

        auto divider = new QFrame;
        divider->setFrameShape(QFrame::HLine);
        scenariosLayout->addWidget(divider);
    }

    auto scenariosList = new QWidget;
    scenariosList->setLayout(scenariosLayout);

    auto scroller = new QScrollArea;
    scroller->setWidgetResizable(true);
    scroller->setWidget(scenariosList);

    setLayout(Ori::Gui::layoutV(
    {
        scroller,
    }));
}

void ScenariosListWidget::scenarioSelected()
{
    auto selected = sender();
    for (int i = 0; i < _scenarioWidgets.size(); i++)
    {
        auto w = _scenarioWidgets.at(i);
        if (w == selected)
            emit currentScenarioSelected(i);
        else
            w->setSelected(false);
    }
}
