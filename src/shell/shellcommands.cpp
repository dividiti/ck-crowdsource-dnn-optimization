#include "appconfig.h"
#include "appmodels.h"
#include "remotedataaccess.h"
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

void ShellCommands::command_querySharedResourcesInfo()
{
    QTextStream cout(stdout);

    auto sharedResourceUrl = AppConfig::sharedResourcesUrl();
    cout << "Shared resources url: " << sharedResourceUrl << endl;

    cout << "Send request..." << endl;

    auto sharedResourcesInfo = RemoteDataAccess::querySharedResourcesInfo(sharedResourceUrl);
    cout << "Public Collective Knowledge Server:" << endl;
    cout << "url: " << sharedResourcesInfo.url << endl;
    cout << "weight: " << sharedResourcesInfo.weight << endl;
    cout << "note: " << sharedResourcesInfo.note << endl;

    cout << "OK" << endl;
}
