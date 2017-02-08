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
#include <QMessageBox>

#define EXPERIMENT_COUNT 1

void setInitialWindowGeometry(QWidget* w)
{
    auto desktop = QApplication::desktop()->availableGeometry(w);
    w->adjustSize();
    // take some more space on the screen than auto-sized
    w->resize(desktop.width()*0.75, w->height());
}

MainWindow::MainWindow(const AppRunParams &runParams, QWidget *parent) : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icon/main"));
    setAttribute(Qt::WA_DeleteOnClose);

    connect(AppEvents::instance(), &AppEvents::onError, this, &MainWindow::onError);

    auto experimentsWidget = new QWidget;
    auto experimentLayout = new QVBoxLayout;
    experimentLayout->setMargin(0);
    experimentLayout->setSpacing(0);
    experimentsWidget->setLayout(experimentLayout);

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
    qDebug() << "Initialize main window";

    CK ck;
    auto models = ck.getModelsByUidOrAll(runParams.modelUid);
    auto engines = ck.getEnginesByUidOrAll(runParams.engineUid);
    auto images = ck.getImagesByUidOrAll(runParams.imagesUid);

    for (auto e: _experiments)
    {
        e->context._models._items = models;
        e->context._engines._items = engines;
        e->context._images._items = images;
        e->context.loadFromConfig();
        e->panel->updateExperimentConditions();
    }

    if (runParams.startImmediately)
        for (auto e: _experiments)
            e->context.startExperiment();
}

void MainWindow::onError(const QString& msg)
{
    QMessageBox::critical(this, windowTitle(), msg);
}
