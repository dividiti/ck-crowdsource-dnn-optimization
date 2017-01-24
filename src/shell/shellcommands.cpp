#include "appconfig.h"
#include "platformfeaturesprovider.h"
#include "scenariosprovider.h"
#include "scenariorunner.h"
#include "shellcommands.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>

ShellCommands::ShellCommands(QObject *parent) : QObject(parent)
{
    _scenariosProvider = new ScenariosProvider(&_network, this);
}

bool ShellCommands::process(const QApplication &app)
{
    QCommandLineParser cmdLine;
    cmdLine.setApplicationDescription(
        "This application let you participate in experiment crowdsourcing " \
        "to collaboratively solve long-standing research problems in computer systems.");
    cmdLine.addHelpOption();
    cmdLine.addVersionOption();

    QCommandLineOption option_querySharedResourcesInfo("1", "Query shared resources info.");
    cmdLine.addOption(option_querySharedResourcesInfo);

    QCommandLineOption option_loadCachedPlatformFeatures("2", "Show cached platform features.");
    cmdLine.addOption(option_loadCachedPlatformFeatures);

    QCommandLineOption option_loadScenariosForCachedFeatures("3", "Load recognition scenarios for cached platform features.");
    cmdLine.addOption(option_loadScenariosForCachedFeatures);

    QCommandLineOption option_saveScenariosForCachedFeatures("4", QString("Save recognition scenarios to cache. Use with option %1.")
        .arg(option_loadScenariosForCachedFeatures.names().first()));
    cmdLine.addOption(option_saveScenariosForCachedFeatures);

    QCommandLineOption option_runCachedScenario("5", "Load cached scenarion.", "scenario index");
    cmdLine.addOption(option_runCachedScenario);

    cmdLine.process(app);

    if (cmdLine.isSet(option_querySharedResourcesInfo))
        command_querySharedRepoInfo();
    else if (cmdLine.isSet(option_loadCachedPlatformFeatures))
        command_showCachedPlatformFeatures();
    else if (cmdLine.isSet(option_loadScenariosForCachedFeatures))
    {
        _saveLoadedScenariosToCache = cmdLine.isSet(option_saveScenariosForCachedFeatures);
        command_loadScenariosForCachedFeatures();
    }
    else if (cmdLine.isSet(option_runCachedScenario))
        command_runCachedScenario(cmdLine.value(option_runCachedScenario));
    else
        return false;

    return true;
}

QTextStream& ShellCommands::cout()
{
    static QTextStream s(stdout);
    return s;
}

void ShellCommands::command_querySharedRepoInfo()
{
    QEventLoop waiter;
    RemoteDataAccess network;
    connect(&network, &RemoteDataAccess::sharedRepoInfoReceived, this, &ShellCommands::sharedRepoInfoReceived);
    connect(&network, &RemoteDataAccess::requestFinished, &waiter, &QEventLoop::quit);
    network.querySharedRepoInfo(AppConfig::sharedResourcesUrl());
    waiter.exec();
}

void ShellCommands::sharedRepoInfoReceived(SharedRepoInfo info)
{
    cout() << info.str() << endl;
}

void ShellCommands::command_showCachedPlatformFeatures()
{
    PlatformFeatures features;
    features.loadFromFile(AppConfig::platformFeaturesCacheFile());
    cout() << features.str() << endl;
}

void ShellCommands::command_loadScenariosForCachedFeatures()
{
    auto url = AppConfig::sharedRepoUrl();
    if (url.isEmpty())
    {
        cout() << "Shared repository url not found in application config" << endl;
        return;
    }
    PlatformFeatures features;
    features.loadFromFile(AppConfig::platformFeaturesCacheFile());
    if (features.isEmpty())
    {
        cout() << "Platform features cache file not found, or it's empty or invalid" << endl;
        return;
    }
    QEventLoop waiter;
    connect(_scenariosProvider, &ScenariosProvider::scenariosReceived, this, &ShellCommands::scenariosReceived);
    connect(&_network, &RemoteDataAccess::requestFinished, &waiter, &QEventLoop::quit);
    _scenariosProvider->queryScenarios(url, features);
    waiter.exec();
}

void ShellCommands::scenariosReceived(RecognitionScenarios scenarios)
{
    if (_saveLoadedScenariosToCache && _scenariosProvider)
    {
        cout() << "Save into" << AppConfig::scenariosCacheFile() << endl;
        _scenariosProvider->saveToCahe(scenarios);
    }
    else
        cout() << scenarios.str() << endl;
}

void ShellCommands::command_runCachedScenario(const QString& scenarionNumber)
{
    RecognitionScenarios scenarios;
    scenarios.loadFromFile(AppConfig::scenariosCacheFile());
    if (scenarios.isEmpty())
    {
        cout() << "Cached recognition scenarios not found or they are empty" << endl;
        return;
    }
    bool ok;
    int scenarioIndex = scenarionNumber.toInt(&ok);
    if (!ok || scenarioIndex < 0 || scenarioIndex >= scenarios.items().size())
    {
        cout() << scenarionNumber << " is not valid scenario index" << endl;
        return;
    }
    ScenarioRunner runner;
    connect(&runner, &ScenarioRunner::scenarioFinished, [&](const QString& error)
    {
        cout() << (error.isEmpty()? "OK": "FAIL: ") << error << endl;
        cout() << "STDOUT:" << endl << runner.readStdout() << endl;
        cout() << "STDERR:" << endl << runner.readStderr() << endl;
    });
    runner.run(scenarios.items().at(scenarioIndex), true);
}
