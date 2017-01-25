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
#include <QMessageBox>
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

    _actionDeleteFiles = Ori::Gui::action(tr("Delete scenario files"), this, SLOT(deleteScenarioFiles()), ":/tools/delete");
    _actionDloadFiles = Ori::Gui::action(tr("Download scenario files"), this, SLOT(downloadsScenarioFiles()), ":/tools/download");
    auto actionShowInfo = Ori::Gui::action(tr("Show scenario info"), this, SLOT(showScenarioInfo()), ":/tools/info");
    // TODO: stop downloading action?

    _downloadingProgress = new QProgressBar;

    setLayout(Ori::Gui::layoutH(
    {
        _selectionFlag,
        Ori::Gui::spacing(16),
        Ori::Gui::layoutV({ labelTitle, labelSize, _downloadingProgress }),
        0,
        Ori::Gui::toolbar({ _actionDeleteFiles, _actionDloadFiles, actionShowInfo })
    }));

    connect(_context->scenariosProvider, &ScenariosProvider::scenarioFileDownloaded, this, &ScenarioItemWidget::fileDownloaded);
    connect(_context->scenariosProvider, &ScenariosProvider::filesDownloadComplete, this, &ScenarioItemWidget::filesDownloadComplete);

    displayFilesStatus();
}

void ScenarioItemWidget::deleteScenarioFiles()
{
    if (Utils::confirmDlg(tr("Confirm deletion")))
    {
        _context->scenariosProvider->deleteScenarioFiles(_scenarioIndex);
        displayFilesStatus();
    }
}

void ScenarioItemWidget::downloadsScenarioFiles()
{
    _context->scenariosProvider->downloadScenarioFiles(_scenarioIndex);
    displayFilesStatus();
}

void ScenarioItemWidget::showScenarioInfo()
{
    Utils::showHtmlInfoWindow(_context->currentScenarios().at(_scenarioIndex).html(), 600, 800);
}

void ScenarioItemWidget::setSelected(bool on)
{
    _selectionFlag->setChecked(on);
}

bool ScenarioItemWidget::selected() const
{
    return _selectionFlag->isChecked();
}

void ScenarioItemWidget::fileDownloaded(int scenarioIndex, int loadedFilesCount)
{
    if (scenarioIndex != _scenarioIndex) return;

    if (!_downloading) displayFilesStatus();

    _downloadingProgress->setValue(loadedFilesCount);
}

void ScenarioItemWidget::filesDownloadComplete(int scenarioIndex)
{
    if (scenarioIndex != _scenarioIndex) return;

    QTimer::singleShot(500, this, SLOT(hideDownloadProgress()));
    displayFilesStatus();
}

void ScenarioItemWidget::hideDownloadProgress()
{
    _downloadingProgress->setVisible(false);
}

void ScenarioItemWidget::displayFilesStatus()
{
    auto filesStatus = _context->scenariosProvider->scenarioDownloadStatus(_scenarioIndex);
    if (!filesStatus)
    {
        _actionDeleteFiles->setVisible(false);
        _actionDloadFiles->setVisible(false);
        _downloadingProgress->setVisible(false);
        return;
    }

    switch (filesStatus->status())
    {
    case ScenarioDownloadStatus::AllLoaded:
        _downloading = false;
        _actionDeleteFiles->setVisible(true);
        _actionDloadFiles->setVisible(false);
        _downloadingProgress->setVisible(false);
        break;

    case ScenarioDownloadStatus::NoFiles:
        _downloading = false;
        _actionDeleteFiles->setVisible(false);
        _actionDloadFiles->setVisible(true);
        _downloadingProgress->setVisible(false);
        break;

    case ScenarioDownloadStatus::IsLoading:
        _downloading = true;
        _actionDeleteFiles->setVisible(false);
        _actionDloadFiles->setVisible(false);
        _downloadingProgress->setMaximum(filesStatus->totalFiles());
        _downloadingProgress->setValue(filesStatus->loadedFiles());
        _downloadingProgress->setVisible(true);
        break;
    }
}

//-----------------------------------------------------------------------------

ScenariosListWidget::ScenariosListWidget(ExperimentContext* context, QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    auto scenariosLayout = new QVBoxLayout;
    int scenariosCount = context->currentScenarios().size();
    for (int i = 0; i < scenariosCount; i++)
    {
        auto scenarioWidget = new ScenarioItemWidget(context, i);
        connect(scenarioWidget, SIGNAL(selectionFlagClicked()), this, SLOT(scenarioSelected()));

        _scenarioWidgets.append(scenarioWidget);

        scenariosLayout->addWidget(scenarioWidget);

        if (i < scenariosCount-1)
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
