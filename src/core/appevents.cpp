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
    emit error(msg);
}
