#include "appconfig.h"
#include "remotedataaccess.h"
#include "shellcommands.h"

#include <QApplication>
#include <QCommandLineParser>
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

    QCommandLineOption option_querySharedResourcesInfo(QStringList() << "s" << "shared-info", "Query shared resources info.");
    cmdLine.addOption(option_querySharedResourcesInfo);

    cmdLine.process(app);

    if (cmdLine.isSet(option_querySharedResourcesInfo))
    {
        command_querySharedResourcesInfo();
        return true;
    }

    return false;
}

QTextStream& ShellCommands::cout()
{
    static QTextStream s(stdout);
    return s;
}

void ShellCommands::command_querySharedResourcesInfo()
{
    auto url = AppConfig::sharedResourcesUrl();
    cout() << "Shared resources url: " << url << endl;
    cout() << "Sending request..." << endl;

    QEventLoop waiter;
    RemoteDataAccess network;
    connect(&network, &RemoteDataAccess::sharedRepoInfoAqcuired, this, &ShellCommands::sharedRepoInfoAqcuired);
    connect(&network, &RemoteDataAccess::requestFinished, &waiter, &QEventLoop::quit);
    network.querySharedRepoInfo(url);
    waiter.exec();

    cout() << "OK" << endl;
}

void ShellCommands::sharedRepoInfoAqcuired(SharedRepoInfo info)
{
    cout() << "Public Collective Knowledge Server:" << endl;
    cout() << "url: " << info.url() << endl;
    cout() << "weight: " << info.weight() << endl;
    cout() << "note: " << info.note() << endl;
}
