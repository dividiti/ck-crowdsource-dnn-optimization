#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appmodels.h"
#include "experimentcontext.h"

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
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private:
    QList<Experiment*> _experiments;

private slots:
    void onError(const QString& msg);
    void onInfo(const QString& msg);
};

#endif // MAINWINDOW_H
