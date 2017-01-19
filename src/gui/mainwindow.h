#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appmodels.h"
#include "remotedataaccess.h"
#include "platformfeaturesprovider.h"

class ExperimentPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QList<ExperimentPanel*> _experimentPanels;
    RemoteDataAccess _network;
    PlatformFeaturesProvider _platformFeaturesProvider;

    void initialize();
    //void collectPlatformFeatures(const QString& sharedRepoUrl);

private slots:
    //void sharedRepoInfoReceived(SharedRepoInfo info);
    void platformFeaturesReceived(PlatformFeatures features);
    void recognitionScenariosReceived(RecognitionScenarios scenarios);
    void onError(const QString& msg);
    void onInfo(const QString& msg);
};

#endif // MAINWINDOW_H
