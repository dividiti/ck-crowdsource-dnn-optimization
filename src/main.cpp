#include "core/appconfig.h"
#include "core/utils.h"
#include "gui/mainwindow.h"
#include "gui/stylesheeteditor.h"
#include "shell/shellcommands.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.addLibraryPath(app.applicationDirPath());
    app.setApplicationVersion("1.0.0.0");

    AppRunParams runParams;
    ShellCommands cmds;
    switch (cmds.process(app))
    {
    case ShellCommands::CommandIgnored:
        break;

    case ShellCommands::CommandFinished:
        return 0;

    case ShellCommands::ParamsAcquired:
        runParams = cmds.appRunParams();
        break;
    }

    qApp->setStyleSheet(Utils::loadTextFromFile(AppConfig::styleSheetFileName()));

    (new MainWindow(runParams))->show();

    if (runParams.runMode == AppRunParams::EditStyle)
        (new StyleSheetEditor)->show();

    return app.exec();
}
