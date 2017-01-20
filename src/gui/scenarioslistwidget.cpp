#include "appmodels.h"
#include "scenarioslistwidget.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QToolBar>
#include <QToolButton>

ScenarioItemWidget::ScenarioItemWidget(const RecognitionScenario* scenario, QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _scenario = scenario;

    auto labelTitle = new QLabel(scenario->title());
    auto f = labelTitle->font();
    f.setBold(true);
    f.setPointSize(f.pointSize()+1);
    labelTitle->setFont(f);

    auto labelSize = new QLabel(scenario->fileSizeMB());

    auto actionDeleteFiles = Ori::Gui::action(tr("Delete scenario files"), this, SLOT(deleteScenarioFiles()), ":/tools/delete");
    auto actionDloadFiles = Ori::Gui::action(tr("Download scenario files"), this, SLOT(downloadsScenarioFiles()), ":/tools/download");
    auto actionShowInfo = Ori::Gui::action(tr("Show scenario info"), this, SLOT(showScenarioInfo()), ":/tools/info");

    setLayout(Ori::Gui::layoutH(
    {
        new QRadioButton,
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
    Utils::showTextInfoWindow(_scenario->str(), 600, 800);
}

void ScenarioItemWidget::useThisScenario()
{
    Utils::infoDlg("TODO useThisScenario");
}

void ScenarioItemWidget::showCurrentScenario()
{
    Utils::infoDlg("TODO showCurrentScenario");
}



ScenariosListWidget::ScenariosListWidget(const RecognitionScenarios& scenarios, QWidget *parent) : QScrollArea(parent)
{
    auto layout = new QVBoxLayout;
    for (const RecognitionScenario& scenario: scenarios.items())
    {
        layout->addWidget(new ScenarioItemWidget(&scenario));

        auto divider = new QFrame;
        divider->setFrameShape(QFrame::HLine);
        layout->addWidget(divider);
    }

    auto content = new QWidget;
    content->setLayout(layout);

    setWidget(content);
}
