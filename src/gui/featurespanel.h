#ifndef FEATURESPANEL_H
#define FEATURESPANEL_H

#include <QWidget>

class ExperimentContext;
class InfoLabel;

class FeaturesPanel : public QWidget
{
    Q_OBJECT
public:
    explicit FeaturesPanel(ExperimentContext *context, QWidget *parent = 0);

    void updateExperimentConditions();

private slots:
    void showPlatformInfo();
    void showScenarioInfo();
    void selectScenario();

private:
    ExperimentContext* _context;
    InfoLabel *_infoPlatform, *_infoScenario, *_infoBatchSize;

    QWidget* makeToolIcon(const QString& tooltip, const char* icon, const char* slot);
};

#endif // FEATURESPANEL_H
