#include "appconfig.h"
#include "platformfeaturesprovider.h"
#include "remotedataaccess.h"
#include "scenariosprovider.h"
#include "shellcommands.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>

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

    cmdLine.process(app);

    if (cmdLine.isSet(option_querySharedResourcesInfo))
    {
        command_querySharedRepoInfo();
        return true;
    }
    if (cmdLine.isSet(option_loadCachedPlatformFeatures))
    {
        command_showCachedPlatformFeatures();
        return true;
    }
    if (cmdLine.isSet(option_loadScenariosForCachedFeatures))
    {
        command_loadScenariosForCachedFeatures();
        return true;
    }

    return false;
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
    PlatformFeatures features;
    features.loadFromFile(AppConfig::platformFeaturesCacheFile());
    if (features.isEmpty())
        return;

    QEventLoop waiter;
    RemoteDataAccess network;
    ScenariosProvider provider(&network);
    connect(&provider, &ScenariosProvider::scenariosReceived, this, &ShellCommands::scenariosReceived);
    connect(&network, &RemoteDataAccess::requestFinished, &waiter, &QEventLoop::quit);
    provider.queryScenarios(AppConfig::sharedRepoUrl(), features);
    waiter.exec();
}

void ShellCommands::scenariosReceived(RecognitionScenarios scenarios)
{
    cout() << scenarios.str() << endl;
}
