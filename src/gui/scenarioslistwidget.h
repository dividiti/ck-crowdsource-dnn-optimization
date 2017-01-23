#ifndef SCENARIOSWIDGET_H
#define SCENARIOSWIDGET_H

#include <QWidget>

class ExperimentContext;

QT_BEGIN_NAMESPACE
class QProgressBar;
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
    void deleteScenarioFiles();
    void downloadsScenarioFiles();
    void showScenarioInfo();
    void useThisScenario();
    void showCurrentScenario();
    void fileDownloaded(int scenarioIndex, int fileIndex, bool success);
    void hideDownloadProgress();

private:
    ExperimentContext* _context;
    int _scenarioIndex;
    QRadioButton* _selectionFlag;
    QProgressBar* _downloadingProgress;
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
