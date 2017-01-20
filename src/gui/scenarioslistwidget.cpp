#include "appmodels.h"
#include "scenarioslistwidget.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>

#define _B(text, icon, slot) Ori::Gui::iconToolButton(text, icon, 24, this, SLOT(slot()))

ScenarioItemWidget::ScenarioItemWidget(const RecognitionScenario* scenario, QWidget *parent) : QWidget(parent)
{
    _scenario = scenario;

    auto labelTitle = new QLabel(scenario->title());
    auto f = labelTitle->font();
    f.setBold(true);
    f.setPointSize(f.pointSize()+1);
    labelTitle->setFont(f);

    auto labelSize = new QLabel(scenario->fileSizeMB());

    setLayout(Ori::Gui::layoutH(
    {
        Ori::Gui::toolbar(
        {
            _B(tr("Use this scenario"), ":/tools/ok_dimmed", useThisScenario),
            _B(tr("It is active scenario"), ":/tools/ok", showCurrentScenario),
        }),
        Ori::Gui::layoutV({ labelTitle, labelSize }),
        0,
        Ori::Gui::spacing(64),
        Ori::Gui::toolbar(
        {
            _B(tr("Delete scenario files"), ":/tools/delete", deleteScenarioFiles),
            _B(tr("Download scenario files"), ":/tools/download", downloadsScenarioFiles),
            _B(tr("Show scenario info"), ":/tools/info", showScenarioInfo),
        })
    }));
}

void ScenarioItemWidget::deleteScenarioFiles()
{
    Utils::infoDlg("deleteScenarioFiles");
}

void ScenarioItemWidget::downloadsScenarioFiles()
{
    Utils::infoDlg("downloadsScenarioFiles");
}

void ScenarioItemWidget::showScenarioInfo()
{
    Utils::showTextInfoWindow(_scenario->str(), 600, 800);
}

void ScenarioItemWidget::useThisScenario()
{
    Utils::infoDlg("useThisScenario");
}

void ScenarioItemWidget::showCurrentScenario()
{
    Utils::infoDlg("showCurrentScenario");
}



ScenariosListWidget::ScenariosListWidget(const RecognitionScenarios& scenarios, QWidget *parent) : QScrollArea(parent)
{
    auto layout = new QVBoxLayout;
    for (const RecognitionScenario& scenario: scenarios.items())
        layout->addWidget(new ScenarioItemWidget(&scenario));

    auto content = new QWidget;
    content->setLayout(layout);

    setWidget(content);
}
