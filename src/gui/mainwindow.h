#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ExperimentPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QList<ExperimentPanel*> _experimentPanels;
};

#endif // MAINWINDOW_H
