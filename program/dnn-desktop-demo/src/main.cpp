#include "core/appevents.h"
#include "core/appconfig.h"
#include "core/appmodels.h"
#include "core/utils.h"
#include "gui/logwindow.h"
#include "gui/mainwindow.h"
#include "shell/shellcommands.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    qInstallMessageHandler(LogWindow::messageHandler);
#endif

    qRegisterMetaType<PredictionResult>("PredictionResult");
    qRegisterMetaType<ImageResult>("ImageResult");

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

    AppEvents::instance()->init();

    qApp->setStyleSheet(Utils::loadTextFromFile(AppConfig::styleSheetFileName()));

    (new MainWindow(runParams))->show();

    return app.exec();
}
