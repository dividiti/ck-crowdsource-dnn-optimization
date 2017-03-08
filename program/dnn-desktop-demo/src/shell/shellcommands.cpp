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

    QCommandLineOption option_models("1", "Enumerate dnn models.");
    cmdLine.addOption(option_models);

    QCommandLineOption option_engines("2", "Enumerate dnn libs.");
    cmdLine.addOption(option_engines);

    QCommandLineOption option_images("3", "Enumerate image datasets.");
    cmdLine.addOption(option_images);

    cmdLine.process(app);

    if (cmdLine.isSet(option_models))
    {
        command_models();
        return CommandFinished;
    }

    if (cmdLine.isSet(option_engines))
    {
        command_engines();
        return CommandFinished;
    }

    if (cmdLine.isSet(option_images))
    {
        command_images();
        return CommandFinished;
    }

    return CommandIgnored;
}

QTextStream& ShellCommands::cout()
{
    static QTextStream s(stdout);
    return s;
}

void ShellCommands::command_models()
{
    for (auto model: CK().queryModels())
        cout() << model.str() << endl;
}

void ShellCommands::command_engines()
{
    for (auto lib: CK().queryEngines())
        cout() << lib.str() << endl;
}

void ShellCommands::command_images()
{
    for (auto dataset: CK().queryImages())
        cout() << dataset.str() << endl;
}
