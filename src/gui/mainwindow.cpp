#include "appconfig.h"
#include "appevents.h"
#include "mainwindow.h"
#include "experimentpanel.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QStatusBar>

void setInitialWindowGeometry(QWidget* w)
{
    auto desktop = QApplication::desktop()->screen()->rect();
    w->adjustSize();
    // take some more space on the screen then auto-sized
    w->resize(desktop.width()*0.75, w->height()*1.1);
    // position at desktop center
    w->move(desktop.center() - w->rect().center());
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icon/main"));

    _experimentPanels.append(new ExperimentPanel(this));
    _experimentPanels.append(new ExperimentPanel(this));

    auto content = new QWidget;
    content->setLayout(new QVBoxLayout);
    for (auto panel: _experimentPanels)
        content->layout()->addWidget(panel);

    setCentralWidget(content);
    setInitialWindowGeometry(this);

    connect(AppEvents::instance(), &AppEvents::error, this, &MainWindow::onError);

    connect(&_network, &RemoteDataAccess::requestFinished, statusBar(), &QStatusBar::clearMessage);
    connect(&_network, &RemoteDataAccess::sharedRepoInfoAqcuired, this, &MainWindow::sharedRepoInfoAqcuired);
    connect(&_network, &RemoteDataAccess::platformFeaturesAqcuired, this, &MainWindow::platformFeaturesAqcuired);
    connect(&_network, &RemoteDataAccess::recognitionScenariosAqcuired, this, &MainWindow::recognitionScenariosAqcuired);

    initialize();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initialize()
{
    qDebug() << "initialize";
    auto sharedRepoUrl = AppConfig::sharedRepoUrl();
    qDebug() << "sharedRepoUrl" << sharedRepoUrl;
    if (sharedRepoUrl.isEmpty())
    {
        qDebug() << "_network.querySharedRepoInfo" << AppConfig::sharedResourcesUrl();
        statusBar()->showMessage(tr("Request for Public Collective Knowledge Server..."));
        _network.querySharedRepoInfo(AppConfig::sharedResourcesUrl());
        return;
    }
    collectPlatformFeatures(sharedRepoUrl);
}

void MainWindow::sharedRepoInfoAqcuired(SharedRepoInfo info)
{
    qDebug() << "sharedRepoInfoAqcuired" << info.url() << info.weight() << info.note();
    AppConfig::setSharedRepoUrl(info.url());
    collectPlatformFeatures(info.url());
}

void MainWindow::collectPlatformFeatures(const QString& sharedRepoUrl)
{
    qDebug() << "collectPlatformFeatures" << sharedRepoUrl;
    statusBar()->showMessage(tr("Collecting platform features..."));
}

void MainWindow::platformFeaturesCollected()
{
    qDebug() << "platformFeaturesCollected";
}

void MainWindow::platformFeaturesAqcuired()
{
    qDebug() << "platformFeaturesAqcuired";
}

void MainWindow::recognitionScenariosAqcuired()
{
    qDebug() << "recognitionScenariosAqcuired";
}

void MainWindow::onError(const QString& msg)
{
    QMessageBox::critical(this, windowTitle(), msg);
}
