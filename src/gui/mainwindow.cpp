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
    auto desktop = QApplication::desktop()->availableGeometry(w);
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

    connect(AppEvents::instance(), &AppEvents::onError, this, &MainWindow::onError);
    connect(AppEvents::instance(), &AppEvents::onInfo, this, &MainWindow::onInfo);

    connect(&_network, &RemoteDataAccess::sharedRepoInfoReceived, this, &MainWindow::sharedRepoInfoReceived);
    connect(&_network, &RemoteDataAccess::scenariosReceived, this, &MainWindow::scenariosReceived);
    connect(&_platformFeaturesProvider, &PlatformFeaturesProvider::platformFeaturesReceived, this, &MainWindow::platformFeaturesReceived);
    connect(&_network, &RemoteDataAccess::requestFinished, statusBar(), &QStatusBar::clearMessage);

    initialize();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initialize()
{
    qDebug() << "initialize";

/*    RecognitionScenarios scenarios;
    scenarios.loadFromFile(AppConfig::scenariosCacheFile());
    if (!scenarios.isEmpty())
        return scenariosReceived(scenarios);

    PlatformFeatures features;
    features.loadFromFile(AppConfig::platformFeaturesCacheFile());
    if (!features.isEmpty())
        return platformFeaturesReceived(features);

    SharedRepoInfo repo;
    repo.loadFromConfig();
    if (!repo.isEmpty())
        return sharedRepoInfoReceived(repo);

    _network.querySharedRepoInfo(AppConfig::sharedResourcesUrl());*/

    SharedRepoInfo repo;
    repo.loadFromConfig();
    if (repo.isEmpty())
        return _network.querySharedRepoInfo(AppConfig::sharedResourcesUrl());

    PlatformFeatures features;
    features.loadFromFile(AppConfig::platformFeaturesCacheFile());
    if (features.isEmpty())
        return _platformFeaturesProvider.queryPlatformFeatures(AppConfig::sharedRepoUrl());

    RecognitionScenarios scenarios;
    scenarios.loadFromFile(AppConfig::scenariosCacheFile());
    if (scenarios.isEmpty())
        return _network.queryScenarios(AppConfig::sharedRepoUrl(), features);
}

void MainWindow::sharedRepoInfoReceived(SharedRepoInfo info)
{
    qDebug() << "sharedRepoInfoReceived" << info.str();
    info.saveToConfig();

    _platformFeaturesProvider.queryPlatformFeatures(info.url());
}

void MainWindow::platformFeaturesReceived(PlatformFeatures features)
{
    qDebug() << "platformFeaturesAqcuired";
    features.saveToFile(AppConfig::platformFeaturesCacheFile());

    _network.queryScenarios(AppConfig::sharedRepoUrl(), features);
}

void MainWindow::scenariosReceived(RecognitionScenarios scenarios)
{
    qDebug() << "recognitionScenariosAqcuired";
    scenarios.saveToFile(AppConfig::scenariosCacheFile());

    loadIntoGui(scenarios);
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

void MainWindow::loadIntoGui(const RecognitionScenarios& scenarios)
{
    // TODO
}
