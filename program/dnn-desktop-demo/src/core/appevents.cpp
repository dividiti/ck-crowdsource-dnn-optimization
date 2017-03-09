#include "appevents.h"

#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QList>
#include <QMutex>

#ifdef Q_OS_WIN
static QList<QString> PROCESSES;
static QMutex PROCESSES_MUTEX;
#endif

// On Linux, child processes are not killed by default, when the parent is done.
// This is why we need top explicitly kill process group. Otherwise, 'classification' processes may live forever, consuming resources.
#ifndef Q_OS_WIN
#include <signal.h>
#include <unistd.h>

bool tolerateSigterm = false;

void killGroup() {
    // Killing processs group if we're the leader
    // (we should be the leader as it is set in the constructor, but still worth checking)
    pid_t pgid = getpgid(0);
    if (getpid() == pgid) {
        tolerateSigterm = true;
        killpg(pgid, SIGTERM);
    }
}

void signalHandler(int) {
    if (tolerateSigterm) {
        tolerateSigterm = false;
        return;
    }
    killGroup();
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
    // Make us the process group leader and register signal handlers.
    setpgid(getpid(), 0);
    signal(SIGQUIT, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);
#endif
    connect(qApp, &QCoreApplication::aboutToQuit, this, &AppEvents::killChildProcesses);
}

void AppEvents::info(const QString& msg)
{
    qInfo() << msg;
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
#ifdef Q_OS_WIN
    PROCESSES_MUTEX.lock();
    for (auto p : PROCESSES) {
        QProcess::execute("taskkill /im " + p + " /f");
    }
    PROCESSES.clear();
    PROCESSES_MUTEX.unlock();
#else
    killGroup();
#endif
}

void AppEvents::registerProcess(const QString &processName) {
#ifdef Q_OS_WIN
    PROCESSES_MUTEX.lock();
    PROCESSES.append(processName);
    PROCESSES_MUTEX.unlock();
#endif
}
