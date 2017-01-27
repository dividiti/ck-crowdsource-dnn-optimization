#ifndef SCENARIOSWIDGET_H
#define SCENARIOSWIDGET_H

#include <QWidget>

class ExperimentContext;

QT_BEGIN_NAMESPACE
//class QAction;
//class QProgressBar;
class QRadioButton;
QT_END_NAMESPACE

class ScenarioItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScenarioItemWidget(ExperimentContext* context, int scenarioIndex, QWidget *parent = 0);

    void setSelected(bool on);
    bool selected() const;

signals:
    void selectionFlagClicked();

private slots:
//    void deleteScenarioFiles();
//    void downloadsScenarioFiles();
    void showScenarioInfo();
//    void fileDownloaded(int scenarioIndex, int loadedFilesCount);
//    void filesDownloadComplete(int scenarioIndex);
//    void hideDownloadProgress();

private:
    ExperimentContext* _context;
    int _scenarioIndex;
    bool _downloading = false;
    QRadioButton* _selectionFlag;
    //QProgressBar* _downloadingProgress;
    //QAction *_actionDeleteFiles, *_actionDloadFiles;

    //void displayFilesStatus();
};


class ScenariosListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScenariosListWidget(ExperimentContext* context, QWidget *parent = 0);

signals:
    void currentScenarioSelected(int index);

private slots:
    void scenarioSelected();

private:
    QList<ScenarioItemWidget*> _scenarioWidgets;
};

#endif // SCENARIOSWIDGET_H
