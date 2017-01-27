#include "appconfig.h"
#include "ck.h"
#include "platformfeaturesprovider.h"
#include "scenariosprovider.h"
#include "scenariorunner.h"
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
    cmdLine.addHelpOption();
    cmdLine.addVersionOption();

    QCommandLineOption option_caffeModels("1", "Query caffe models.");
    cmdLine.addOption(option_caffeModels);

    cmdLine.process(app);

    if (cmdLine.isSet(option_caffeModels))
        command_caffeModels();
    else
        return false;

    return true;
}

QTextStream& ShellCommands::cout()
{
    static QTextStream s(stdout);
    return s;
}

void ShellCommands::command_caffeModels()
{
    for (auto m: CK().queryCaffeModels())
        cout() << m.uid << ": " << m.name << endl;
}
