#include "appmodels.h"
#include "experimentcontext.h"
#include "scenarioslistwidget.h"
#include "scenariosprovider.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QProgressBar>
#include <QRadioButton>
#include <QScrollArea>
#include <QTimer>
#include <QToolBar>

ScenarioItemWidget::ScenarioItemWidget(ExperimentContext *context, int scenarioIndex, QWidget *parent) : QWidget(parent)
{
    _context = context;
    _scenarioIndex = scenarioIndex;

    const RecognitionScenario& scenario = _context->currentScenarios().at(_scenarioIndex);

    auto labelTitle = new QLabel(scenario.title());
    auto f = labelTitle->font();
    f.setBold(true);
    f.setPointSize(f.pointSize()+1);
    labelTitle->setFont(f);

    auto labelSize = new QLabel(scenario.fileSizeMB());

    _selectionFlag = new QRadioButton;
    _selectionFlag->setChecked(scenarioIndex == context->currentScenarioIndex());
    connect(_selectionFlag, SIGNAL(clicked(bool)), this, SIGNAL(selectionFlagClicked()));

    auto actionDeleteFiles = Ori::Gui::action(tr("Delete scenario files"), this, SLOT(deleteScenarioFiles()), ":/tools/delete");
    auto actionDloadFiles = Ori::Gui::action(tr("Download scenario files"), this, SLOT(downloadsScenarioFiles()), ":/tools/download");
    auto actionShowInfo = Ori::Gui::action(tr("Show scenario info"), this, SLOT(showScenarioInfo()), ":/tools/info");

    _downloadingProgress = new QProgressBar;
    _downloadingProgress->setMaximum(scenario.files().size());
    _downloadingProgress->setVisible(false);

    setLayout(Ori::Gui::layoutH(
    {
        _selectionFlag,
        Ori::Gui::spacing(16),
        Ori::Gui::layoutV({ labelTitle, labelSize, _downloadingProgress }),
        0,
        Ori::Gui::toolbar({ actionDeleteFiles, actionDloadFiles, actionShowInfo })
    }));

    bool filesLoaded = scenario.allFilesAreLoaded();
    actionDeleteFiles->setVisible(filesLoaded);
    actionDloadFiles->setVisible(!filesLoaded);
}

void ScenarioItemWidget::deleteScenarioFiles()
{
    Utils::infoDlg("TODO deleteScenarioFiles");
}

void ScenarioItemWidget::downloadsScenarioFiles()
{
    connect(_context->scenariosProvider, &ScenariosProvider::scenarioFileDownloaded, this, &ScenarioItemWidget::fileDownloaded);
    _downloadingProgress->setVisible(true);
    _downloadingProgress->setValue(0);
    _context->scenariosProvider->downloadScenarioFiles(_scenarioIndex);
}

void ScenarioItemWidget::showScenarioInfo()
{
    Utils::showHtmlInfoWindow(_context->currentScenarios().at(_scenarioIndex).html(), 600, 800);
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

void ScenarioItemWidget::fileDownloaded(int scenarioIndex, int fileIndex, bool success)
{
    if (scenarioIndex != _scenarioIndex) return;
    _downloadingProgress->setValue(_downloadingProgress->value()+1);
    qDebug() << _downloadingProgress->value();
    if (_downloadingProgress->value() >= _downloadingProgress->maximum())
    {
        QTimer::singleShot(500, this, SLOT(hideDownloadProgress()));
        disconnect(_context->scenariosProvider, &ScenariosProvider::scenarioFileDownloaded, this, &ScenarioItemWidget::fileDownloaded);
    }
}

void ScenarioItemWidget::hideDownloadProgress()
{
    _downloadingProgress->setVisible(false);
}

//-----------------------------------------------------------------------------

ScenariosListWidget::ScenariosListWidget(ExperimentContext* context, QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    auto scenariosLayout = new QVBoxLayout;
    for (int i = 0; i < context->currentScenarios().size(); i++)
    {
        auto scenarioWidget = new ScenarioItemWidget(context, i);
        connect(scenarioWidget, SIGNAL(selectionFlagClicked()), this, SLOT(scenarioSelected()));

        _scenarioWidgets.append(scenarioWidget);

        scenariosLayout->addWidget(scenarioWidget);
        scenariosLayout->addWidget(Utils::makeDivider());
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

    adjustSize();
    // take some more width than autosized, for scroolbars
    resize(width()*1.5, height()*1.5);
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
