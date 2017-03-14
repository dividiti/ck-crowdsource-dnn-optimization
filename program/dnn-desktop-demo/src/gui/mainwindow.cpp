#include "appconfig.h"
#include "appevents.h"
#include "logwindow.h"
#include "mainwindow.h"
#include "experimentpanel.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QShortcut>

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

    setCentralWidget(experimentsWidget);
    setInitialWindowGeometry(this);

    auto desktop = QApplication::desktop()->availableGeometry(this);
    move(desktop.center() - rect().center());

    for (auto e: _experiments) {
        e->panel->updateExperimentConditions();
    }

#ifdef Q_OS_WIN
    auto shortcut = new QShortcut(QKeySequence(Qt::Key_F12), this);
    shortcut->setContext(Qt::ApplicationShortcut);
    connect(shortcut, SIGNAL(activated()), this, SLOT(showLog()));
#endif
}

MainWindow::~MainWindow() {
    for (auto e: _experiments) {
        delete e;
    }
    LogWindow::destroyInstance();
}

void MainWindow::onError(const QString& msg) {
    QMessageBox::critical(this, windowTitle(), msg);
}

void MainWindow::showLog() {
    LogWindow::instance()->show();
}
