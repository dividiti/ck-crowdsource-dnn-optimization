#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appmodels.h"
#include "experimentcontext.h"
//#include "remotedataaccess.h"
//#include "platformfeaturesprovider.h"
#include "scenariosprovider.h"

class ExperimentPanel;

class Experiment
{
public:
    ExperimentPanel* panel;
    ExperimentContext context;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QList<Experiment*> _experiments;
//    RemoteDataAccess _network;
//    PlatformFeaturesProvider _platformFeaturesProvider;
    ScenariosProvider* _scenariosProvider;


    void initialize();
    void updateExperimentConditions();

private slots:
//    void sharedRepoInfoReceived(SharedRepoInfo info);
//    void platformFeaturesReceived(PlatformFeatures features);
//    void scenariosReceived(RecognitionScenarios scenarios);
    void onError(const QString& msg);
    void onInfo(const QString& msg);
};

#endif // MAINWINDOW_H
