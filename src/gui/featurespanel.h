#ifndef FEATURESPANEL_H
#define FEATURESPANEL_H

#include <QWidget>
#include <QPointer>

class ExperimentContext;
class InfoLabel;
class ScenariosListWidget;

class FeaturesPanel : public QWidget
{
    Q_OBJECT
public:
    explicit FeaturesPanel(ExperimentContext *context, QWidget *parent = 0);
    ~FeaturesPanel();

    void updateExperimentConditions();

private slots:
    void showPlatformInfo();
    void showScenarioInfo();
    void selectScenario();
    void currentScenarioSelected(int index);

private:
    ExperimentContext* _context;
    QPointer<ScenariosListWidget> _scenariosWindow;
    InfoLabel *_infoPlatform, *_infoScenario, *_infoBatchSize;

    QWidget* makeLink(const QString &text, const QString& tooltip, const char* slot);
};

#endif // FEATURESPANEL_H
