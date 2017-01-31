#include "appconfig.h"
#include "appevents.h"
#include "ck.h"
#include "mainwindow.h"
#include "experimentpanel.h"
#include "utils.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QMessageBox>
#include <QStatusBar>

#define EXPERIMENT_COUNT 1

void setInitialWindowGeometry(QWidget* w)
{
    auto desktop = QApplication::desktop()->availableGeometry(w);
    w->adjustSize();
    // take some more space on the screen than auto-sized
    w->resize(desktop.width()*0.75, w->height()*2);
}

MainWindow::MainWindow(const AppRunParams &runParams, QWidget *parent) : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icon/main"));
    setAttribute(Qt::WA_DeleteOnClose);

    connect(AppEvents::instance(), &AppEvents::onError, this, &MainWindow::onError);
    connect(AppEvents::instance(), &AppEvents::onInfo, this, &MainWindow::onInfo);

    auto experimentsWidget = new QWidget;
    experimentsWidget->setLayout(new QVBoxLayout);

    for (int i = 0; i < EXPERIMENT_COUNT; i++)
    {
        auto e = new Experiment;
        e->context._experimentIndex = i;
        e->panel = new ExperimentPanel(&e->context);
        _experiments.append(e);

        experimentsWidget->layout()->addWidget(e->panel);
    }

    setCentralWidget(experimentsWidget);
    setInitialWindowGeometry(this);
    Utils::moveToDesktopCenter(this);

    initialize(runParams);
}

MainWindow::~MainWindow()
{
    for (auto e: _experiments) delete e;

    Utils::closeAllInfoWindows();
}

void MainWindow::initialize(const AppRunParams &runParams)
{
    qDebug() << "initialize main window";

    CK ck;
    auto engines = ck.getCafeeLibByUidOrAll(runParams.engineUid);
    auto models = ck.getCafeeModelByUidOrAll(runParams.modelUid);

    for (auto e: _experiments)
    {
        e->context._engines._items = engines;
        e->context._models._items = models;
        e->context.loadFromConfig();
        e->panel->updateExperimentConditions();
    }

    if (runParams.startImmediately)
        for (auto e: _experiments)
            e->context.startExperiment();
}

void MainWindow::onError(const QString& msg)
{
    statusBar()->clearMessage();
    QMessageBox::critical(this, windowTitle(), msg);
}

void MainWindow::onInfo(const QString& msg)
{
    statusBar()->showMessage(msg);
}
