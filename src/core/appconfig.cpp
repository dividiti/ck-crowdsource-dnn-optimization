#include "appconfig.h"
#include "remotedataaccess.h"

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

QString AppConfig::remoteServerUrl()
{
    QSettings cfg(configFileName(), QSettings::IniFormat);

    auto url = cfg.value(ConfigKeys::remoteServer).toString();
    if (url.isEmpty())
    {
        url = RemoteDataAccess::querySharedResourcesInfo(sharedResourcesUrl()).url;
        cfg.setValue(ConfigKeys::remoteServer, url);
    }

    return url;
}

QString AppConfig::configFileName()
{
    return qApp->applicationDirPath() + QDir::separator() + "app.conf";
}
