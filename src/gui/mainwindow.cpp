#include "appconfig.h"
#include "appevents.h"
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

#define EXPERIMENT_COUNT 2

void setInitialWindowGeometry(QWidget* w)
{
    auto desktop = QApplication::desktop()->availableGeometry(w);
    w->adjustSize();
    // take some more space on the screen than auto-sized
    w->resize(desktop.width()*0.75, w->height()*1.1);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icon/main"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto experimentsWidget = new QWidget;
    experimentsWidget->setLayout(new QVBoxLayout);

    _scenariosProvider = new ScenariosProvider(&_network, this);

    for (int i = 0; i < EXPERIMENT_COUNT; i++)
    {
        auto e = new Experiment;
        e->context.platformFeaturesProvider = &_platformFeaturesProvider;
        e->context.scenariosProvider = _scenariosProvider;
        e->panel = new ExperimentPanel(&e->context);
        _experiments.append(e);

        experimentsWidget->layout()->addWidget(e->panel);
    }

    setCentralWidget(experimentsWidget);
    setInitialWindowGeometry(this);
    Utils::moveToDesktopCenter(this);

    connect(AppEvents::instance(), &AppEvents::onError, this, &MainWindow::onError);
    connect(AppEvents::instance(), &AppEvents::onInfo, this, &MainWindow::onInfo);

    connect(&_network, &RemoteDataAccess::sharedRepoInfoReceived, this, &MainWindow::sharedRepoInfoReceived);
    connect(_scenariosProvider, &ScenariosProvider::scenariosReceived, this, &MainWindow::scenariosReceived);
    connect(&_platformFeaturesProvider, &PlatformFeaturesProvider::platformFeaturesReceived, this, &MainWindow::platformFeaturesReceived);
    connect(&_network, &RemoteDataAccess::requestFinished, statusBar(), &QStatusBar::clearMessage);

    initialize();
}

MainWindow::~MainWindow()
{
    for (auto e: _experiments) delete e;

    Utils::closeAllInfoWindows();
}

void MainWindow::initialize()
{
    qDebug() << "initialize";

    SharedRepoInfo repo;
    repo.loadFromConfig();
    if (repo.isEmpty())
        return _network.querySharedRepoInfo(AppConfig::sharedResourcesUrl());

    auto features = _platformFeaturesProvider.loadFromCache();
    if (features.isEmpty())
        return _platformFeaturesProvider.queryPlatformFeatures(AppConfig::sharedRepoUrl());
    _platformFeaturesProvider.setCurrent(features);

    auto scenarios = _scenariosProvider->loadFromCache();
    if (scenarios.isEmpty())
        return _scenariosProvider->queryScenarios(AppConfig::sharedRepoUrl(), features);
    _scenariosProvider->setCurrentList(scenarios);

    updateExperimentConditions();
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
    _platformFeaturesProvider.saveToCache(features);
    _platformFeaturesProvider.setCurrent(features);
    _scenariosProvider->queryScenarios(AppConfig::sharedRepoUrl(), features);
}

void MainWindow::scenariosReceived(RecognitionScenarios scenarios)
{
    qDebug() << "recognitionScenariosAqcuired";
    _scenariosProvider->saveToCahe(scenarios);
    _scenariosProvider->setCurrentList(scenarios);
    updateExperimentConditions();
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

void MainWindow::updateExperimentConditions()
{
    for (auto e: _experiments)
       e->panel->updateExperimentConditions();
}
