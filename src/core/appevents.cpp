#include "appevents.h"

#include <QDebug>

AppEvents* AppEvents::instance()
{
    static AppEvents obj;
    return &obj;
}

void AppEvents::reportError(const QString& msg)
{
    qCritical() << "ERROR:" << msg;

    // TODO: raise signal for GUI-app
    // TODO: log to file?
}
