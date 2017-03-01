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

private slots:
    void startExperiment();
    void stopExperiment();
    void experimentStarted();
    void experimentFinished();

private:
    ExperimentContext* _context;
    FeaturesPanel* _featuresPanel;
    QWidget *_buttonStart, *_buttonStop;

    void enableControls(bool on);

    void adjustSidebar(QWidget* w);
};

#endif // EXPERIMENTPANEL_H
