#include "gui/mainwindow.h"
#include "shell/shellcommands.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.addLibraryPath(app.applicationDirPath());
    app.setApplicationVersion("1.0.0.0");

    if (ShellCommands::process(app))
    {
        return 0;

        // messages like 'QThreadStorage: Thread 0x262a700 exited after QThreadStorage 2 destroyed'
        // will be reported after commands related to web, because of network is asynchronous in Qt,
        // but main event loop is not started yet at this moment (it starts by app.exec())
    }

    MainWindow w;
    w.show();

    return app.exec();
}
