#include "appconfig.h"
#include "appevents.h"
#include "mainwindow.h"
#include "experimentpanel.h"
#include "footerpanel.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>

#define EXPERIMENT_COUNT 1

void setInitialWindowGeometry(QWidget* w) {
    auto desktop = QApplication::desktop()->availableGeometry(w);
    w->adjustSize();
    // take some more space on the screen than auto-sized
    w->resize(desktop.width()*0.75, w->height());
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowIcon(QIcon(":/icon/main"));
    setAttribute(Qt::WA_DeleteOnClose);

    connect(AppEvents::instance(), &AppEvents::onError, this, &MainWindow::onError);
    connect(AppEvents::instance(), &AppEvents::onInfo, this, &MainWindow::onInfo);

    auto experimentsWidget = new QWidget;
    auto experimentLayout = new QVBoxLayout;
    experimentLayout->setMargin(0);
    experimentLayout->setSpacing(0);
    experimentsWidget->setLayout(experimentLayout);

    for (int i = 0; i < EXPERIMENT_COUNT; i++) {
        auto e = new Experiment;
        e->panel = new ExperimentPanel(&e->context);
        _experiments.append(e);
        experimentsWidget->layout()->addWidget(e->panel);
    }

    experimentsWidget->layout()->addWidget(new FooterPanel(&(_experiments.at(0)->context)));

    setCentralWidget(experimentsWidget);
    setInitialWindowGeometry(this);

    auto desktop = QApplication::desktop()->availableGeometry(this);
    move(desktop.center() - rect().center());

    for (auto e: _experiments) {
        e->panel->updateExperimentConditions();
    }
}

MainWindow::~MainWindow() {
    for (auto e: _experiments) {
        delete e;
    }
}

void MainWindow::onError(const QString& msg) {
    QMessageBox::critical(this, windowTitle(), msg);
}

void MainWindow::onInfo(const QString& msg) {
    QMessageBox::information(this, windowTitle(), msg);
}
