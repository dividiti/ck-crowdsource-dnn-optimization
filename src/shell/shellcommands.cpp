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

    QCommandLineOption option_caffeModels("1", "Enumerate caffe models.");
    cmdLine.addOption(option_caffeModels);

    QCommandLineOption option_caffeLibs("2", "Enumerate caffe libs.");
    cmdLine.addOption(option_caffeLibs);

    QCommandLineOption option_runRecognition("3", "Run recognition with specified model.");
    cmdLine.addOption(option_runRecognition);

    QCommandLineOption option_editStyle("4", "Edit application style.");
    cmdLine.addOption(option_editStyle);

    QCommandLineOption option_recognitionEngine("engine", QString("caffe-lib uid to run recognition (for command %1).")
                                               .arg(option_runRecognition.names().first()), "uid");
    cmdLine.addOption(option_recognitionEngine);

    QCommandLineOption option_recognitionModel("model", QString("caffe-model uid to run recognition (for command %1).")
                                               .arg(option_runRecognition.names().first()), "uid");
    cmdLine.addOption(option_recognitionModel);

    cmdLine.process(app);

    if (cmdLine.isSet(option_caffeModels))
    {
        command_caffeModels();
        return CommandFinished;
    }

    if (cmdLine.isSet(option_caffeLibs))
    {
        command_caffeLibs();
        return CommandFinished;
    }

    if (cmdLine.isSet(option_editStyle))
    {
        _appParams.runMode = AppRunParams::EditStyle;
        return ParamsAcquired;
    }

    if (cmdLine.isSet(option_runRecognition))
    {
        _appParams.engineUid = cmdLine.value(option_recognitionEngine);
        _appParams.modelUid = cmdLine.value(option_recognitionModel);
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

void ShellCommands::command_caffeLibs()
{
    for (auto lib: CK().queryCaffeLibs())
        cout() << lib.str() << endl;
}

void ShellCommands::command_caffeModels()
{
    for (auto model: CK().queryCaffeModels())
        cout() << model.str() << endl;
}
