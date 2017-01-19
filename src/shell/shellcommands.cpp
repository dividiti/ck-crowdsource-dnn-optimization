#include "appconfig.h"
#include "platformfeaturesprovider.h"
#include "remotedataaccess.h"
#include "shellcommands.h"
#include "utils.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

ShellCommands::ShellCommands(QObject *parent) : QObject(parent)
{
}

bool ShellCommands::process(const QApplication &app)
{
    QCommandLineParser cmdLine;
    cmdLine.setApplicationDescription(
        "This application let you participate in experiment crowdsourcing " \
        "to collaboratively solve long-standing research problems in computer systems.");
    cmdLine.addHelpOption();
    cmdLine.addVersionOption();

    QCommandLineOption option_querySharedResourcesInfo("qsi", "Query shared resources info.");
    cmdLine.addOption(option_querySharedResourcesInfo);

    QCommandLineOption option_loadCachedPlatformFeatures("cpf", "Show cached platform features.");
    cmdLine.addOption(option_loadCachedPlatformFeatures);

    QCommandLineOption option_loadScenariosForCachedFeatures("scpf", "Load recognition scenarios for cached platform features.");
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
    auto url = AppConfig::sharedResourcesUrl();
    cout() << "Shared resources url: " << url << endl;
    cout() << "Sending request..." << endl;

    QEventLoop waiter;
    RemoteDataAccess network;
    connect(&network, &RemoteDataAccess::sharedRepoInfoReceived, this, &ShellCommands::sharedRepoInfoReceived);
    connect(&network, &RemoteDataAccess::requestFinished, &waiter, &QEventLoop::quit);
    network.querySharedRepoInfo(url);
    waiter.exec();
}

void ShellCommands::sharedRepoInfoReceived(SharedRepoInfo info)
{
    cout() << "Public Collective Knowledge Server:" << endl;
    cout() << "url: " << info.url() << endl;
    cout() << "weight: " << info.weight() << endl;
    cout() << "note: " << info.note() << endl;
}

bool ShellCommands::loadCachedPlatformFeatures(PlatformFeatures& features)
{
    PlatformFeaturesProvider provider;
    auto res = provider.loadCachedPlatformFeatures(features);
    bool ok = res.isEmpty();
    if (!ok)
        cout() << "ERROR: " << res << endl;
    return ok;
}

void ShellCommands::command_showCachedPlatformFeatures()
{
    PlatformFeatures features;
    if (loadCachedPlatformFeatures(features))
        cout() << Utils::jsonObjectToString(features.json) << endl;
}

void ShellCommands::command_loadScenariosForCachedFeatures()
{
    PlatformFeatures features;
    if (!loadCachedPlatformFeatures(features))
        return;

    auto url = AppConfig::sharedRepoUrl();
    if (url.isEmpty())
    {
        cout() << "ERROR: Shared reposources url is not set" << endl;
        return;
    }

    cout() << "Sending request to " << url << "..." << endl;

    QEventLoop waiter;
    RemoteDataAccess network;
    connect(&network, &RemoteDataAccess::recognitionScenariosReceived, this, &ShellCommands::recognitionScenariosReceived);
    connect(&network, &RemoteDataAccess::requestFinished, &waiter, &QEventLoop::quit);
    network.queryRecognitionScenarios(url, features);
    waiter.exec();
}

void ShellCommands::recognitionScenariosReceived(RecognitionScenarios scenarios)
{
    cout() << "Scenarios count: " << scenarios.jsons.size() << endl;
    for (auto json: scenarios.jsons)
    {
        cout() << "-----------------------------" << endl;
        cout() << Utils::jsonObjectToString(json) << endl;
    }
}
