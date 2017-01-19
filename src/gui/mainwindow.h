#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appmodels.h"
#include "remotedataaccess.h"

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

    void initialize();
    void collectPlatformFeatures(const QString& sharedRepoUrl);

private slots:
    void sharedRepoInfoAqcuired(SharedRepoInfo info);
    void platformFeaturesCollected();
    void platformFeaturesAqcuired();
    void recognitionScenariosAqcuired();
    void onError(const QString& msg);
};

#endif // MAINWINDOW_H
