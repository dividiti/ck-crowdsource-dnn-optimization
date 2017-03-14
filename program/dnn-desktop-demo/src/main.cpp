#include "core/appevents.h"
#include "core/appconfig.h"
#include "core/appmodels.h"
#include "gui/logwindow.h"
#include "gui/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<PredictionResult>("PredictionResult");
    qRegisterMetaType<ImageResult>("ImageResult");

    QApplication app(argc, argv);
    app.addLibraryPath(app.applicationDirPath());
    app.setApplicationVersion("1.1.0.0");

    AppEvents::instance()->init();

    qApp->setStyleSheet(AppConfig::styleSheet());

    (new MainWindow())->show();

    return app.exec();
}
