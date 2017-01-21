#include "gui/mainwindow.h"
#include "shell/shellcommands.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.addLibraryPath(app.applicationDirPath());
    app.setApplicationVersion("1.0.0.0");

    ShellCommands cmds;
    if (cmds.process(app))
        return 0;

    (new MainWindow)->show();

    return app.exec();
}
