#include "appconfig.h"
#include "ck.h"
#include "shellcommands.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>

ShellCommands::ShellCommands(QObject *parent) : QObject(parent)
{
}

ShellCommands::Result ShellCommands::process(const QApplication &app)
{
    QCommandLineParser cmdLine;
    cmdLine.addHelpOption();
    cmdLine.addVersionOption();

    QCommandLineOption option_caffeModels("1", "Query caffe models.");
    cmdLine.addOption(option_caffeModels);

    QCommandLineOption option_runRecognition("2", "Run recognition with specified model.", "model_uid");
    cmdLine.addOption(option_runRecognition);

    cmdLine.process(app);

    if (cmdLine.isSet(option_caffeModels))
    {
        command_caffeModels();
        return CommandFinished;
    }

    if (cmdLine.isSet(option_runRecognition))
    {
        _appParams.dnnModelUid = cmdLine.value(option_runRecognition);
        _appParams.startImmediately = true;
        return ParamsAcquired;
    }

    return CommandIgnored;
}

QTextStream& ShellCommands::cout()
{
    static QTextStream s(stdout);
    return s;
}

void ShellCommands::command_caffeModels()
{
    for (auto m: CK().queryCaffeModels())
        cout() << m.str() << endl;
}
