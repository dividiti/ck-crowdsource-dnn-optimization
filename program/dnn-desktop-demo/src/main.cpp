#include "core/appevents.h"
#include "core/appconfig.h"
#include "core/appmodels.h"
#include "core/utils.h"
#include "gui/logwindow.h"
#include "gui/mainwindow.h"

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
    app.setApplicationVersion("1.1.0.0");

    AppEvents::instance()->init();

    qApp->setStyleSheet(Utils::loadTextFromFile(AppConfig::styleSheetFileName()));

    (new MainWindow())->show();

    return app.exec();
}
