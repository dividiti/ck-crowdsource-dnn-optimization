#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appmodels.h"
#include "experimentcontext.h"
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
    explicit MainWindow(const AppRunParams& runParams, QWidget *parent = 0);
    ~MainWindow();

private:
    QList<Experiment*> _experiments;
    ScenariosProvider* _scenariosProvider;

    void initialize(const AppRunParams &runParams);

private slots:
    void onError(const QString& msg);
    void onInfo(const QString& msg);
};

#endif // MAINWINDOW_H
