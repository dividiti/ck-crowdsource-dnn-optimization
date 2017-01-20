#ifndef EXPERIMENTPANEL_H
#define EXPERIMENTPANEL_H

#include <QFrame>

class ExperimentContext;
class FeaturesPanel;

class ExperimentPanel : public QFrame
{
    Q_OBJECT

public:
    explicit ExperimentPanel(ExperimentContext* context, QWidget *parent = 0);

    void updateExperimentConditions();

private:
    FeaturesPanel* _featuresPanel;
};

#endif // EXPERIMENTPANEL_H
