#ifndef SCENARIOSWIDGET_H
#define SCENARIOSWIDGET_H

#include <QWidget>

class RecognitionScenario;
class RecognitionScenarios;

QT_BEGIN_NAMESPACE
class QRadioButton;
QT_END_NAMESPACE

class ScenarioItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScenarioItemWidget(const RecognitionScenario *scenario, QWidget *parent = 0);

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

private:
    const RecognitionScenario* _scenario;
    QRadioButton* _selectionFlag;
};


class ScenariosListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScenariosListWidget(const RecognitionScenarios &scenarios, int currentScenario, QWidget *parent = 0);

signals:
    void currentScenarioSelected(int index);

private slots:
    void scenarioSelected();

private:
    QList<ScenarioItemWidget*> _scenarioWidgets;
};

#endif // SCENARIOSWIDGET_H
