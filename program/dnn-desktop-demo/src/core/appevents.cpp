#include "appevents.h"
#include "appconfig.h"

#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QList>
#include <QMutex>
#include <QFile>

#ifndef Q_OS_WIN
#include <signal.h>
#include <unistd.h>

void signalHandler(int) {
    AppEvents::instance()->killChildProcesses();
    QCoreApplication::exit(1);
}
#endif

AppEvents* AppEvents::instance()
{
    static AppEvents obj;
    return &obj;
}

void AppEvents::init() {
#ifndef Q_OS_WIN
    signal(SIGQUIT, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);
#endif
    connect(qApp, &QCoreApplication::aboutToQuit, this, &AppEvents::killChildProcesses);
}

void AppEvents::info(const QString& msg)
{
    qDebug() << msg;
    emit instance()->onInfo(msg);
}

void AppEvents::warning(const QString& msg)
{
    qWarning() << msg;
}

void AppEvents::error(const QString& msg)
{
    qCritical() << "ERROR:" << msg;
    emit instance()->onError(msg);
}

void AppEvents::killChildProcesses() {
    qDebug() << "Telling spawned child processes to stop...";
    QFile finisherFile(AppConfig::finisherFilePath());
    finisherFile.open(QIODevice::WriteOnly);
    finisherFile.close();
}
