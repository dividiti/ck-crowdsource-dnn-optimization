#ifndef SCENARIOSWIDGET_H
#define SCENARIOSWIDGET_H

#include <QScrollArea>

class RecognitionScenario;
class RecognitionScenarios;

class ScenarioItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScenarioItemWidget(const RecognitionScenario *scenario, QWidget *parent = 0);

signals:
    void checked();

private slots:
    void deleteScenarioFiles();
    void downloadsScenarioFiles();
    void showScenarioInfo();
    void useThisScenario();
    void showCurrentScenario();

private:
    const RecognitionScenario* _scenario;
};


class ScenariosListWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit ScenariosListWidget(const RecognitionScenarios &scenarios, QWidget *parent = 0);
};

#endif // SCENARIOSWIDGET_H
