#include "appconfig.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QDebug>

namespace ConfigKeys
{
    const QString remoteServer("remote_server_url");

} // namespace ConfigKeys

//-----------------------------------------------------------------------------

QString AppConfig::sharedResourcesUrl()
{
    return QStringLiteral("http://cTuning.org/shared-computing-resources-json/ck.json");
}

QString AppConfig::sharedRepoUrl()
{
    QSettings cfg(configFileName(), QSettings::IniFormat);
    return cfg.value(ConfigKeys::remoteServer).toString();
}

void AppConfig::setSharedRepoUrl(const QString& url)
{
    QSettings cfg(configFileName(), QSettings::IniFormat);
    cfg.setValue(ConfigKeys::remoteServer, url);
}

QString AppConfig::configFileName()
{
    return qApp->applicationDirPath() + QDir::separator() + "app.conf";
}
