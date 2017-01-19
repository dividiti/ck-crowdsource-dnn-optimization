#include "appconfig.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QDebug>

namespace ConfigKeys
{
    const QString remoteServer("remote_server_url");
    const QString email("email");

} // namespace ConfigKeys

//-----------------------------------------------------------------------------

QSettings& AppConfig::config()
{
    static QSettings cfg(configFileName(), QSettings::IniFormat);
    return cfg;
}

QString AppConfig::sharedResourcesUrl()
{
    return QStringLiteral("http://cTuning.org/shared-computing-resources-json/ck.json");
}

QString AppConfig::sharedRepoUrl()
{
    return config().value(ConfigKeys::remoteServer).toString();
}

void AppConfig::setSharedRepoUrl(const QString& url)
{
    config().setValue(ConfigKeys::remoteServer, url);
}

QString AppConfig::email()
{
    return config().value(ConfigKeys::email).toString();
}

QString AppConfig::configFileName()
{
    return qApp->applicationDirPath() + QDir::separator() + "app.conf";
}

QString AppConfig::platformFeaturesCacheFile()
{
    return qApp->applicationDirPath() + QDir::separator() + "platformFeaturesFile.json";
}
