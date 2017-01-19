#include "gui/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
