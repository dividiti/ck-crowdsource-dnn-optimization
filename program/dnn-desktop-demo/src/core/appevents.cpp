#include "appevents.h"
#include "appconfig.h"

#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QList>
#include <QMutex>
#include <QFile>

AppEvents* AppEvents::instance()
{
    static AppEvents obj;
    return &obj;
}

void AppEvents::init() {
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
