#ifndef EXPERIMENTPANEL_H
#define EXPERIMENTPANEL_H

#include "../core/appmodels.h"
#include <QFrame>
#include <QProcess>

class ExperimentContext;
class FeaturesPanel;
class ImageResult;

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class ExperimentPanel : public QFrame
{
    Q_OBJECT

public:

    explicit ExperimentPanel(ExperimentContext* context, QWidget *parent = 0);

    void updateExperimentConditions();

private slots:
    void startExperiment();
    void startOver();
    void stopExperiment();
    void experimentStarted();
    void experimentFinished();
    void publishStarted();
    void publishFinished(bool);
    void modeChanged(Mode);

private:
    ExperimentContext* _context;
    FeaturesPanel* _featuresPanel;
    QPushButton* _buttonStart;
    QPushButton* _buttonStop;
    QPushButton* _buttonStartOver;
    bool _restart = false;

    void enableControls(bool on);
    void adjustSidebar(QWidget* w);
    bool canResume() const;
};

#endif // EXPERIMENTPANEL_H
