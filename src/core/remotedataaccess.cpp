#include "appevents.h"
#include "remotedataaccess.h"
#include "webclient.h"

#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

SharedResourcesInfo RemoteDataAccess::querySharedResourcesInfo(const QString& url)
{
    SharedResourcesInfo result;

    auto res = WebClient::loadStringData(url);
    if (res.failed())
    {
        AppEvents::instance()->reportError(
            qApp->tr("Unable to load shared resources from %1: %2").arg(url).arg(res.error()));
        return result;
    }

    QJsonParseError error;
    auto json = QJsonDocument::fromJson(res.data().toUtf8(), &error);
    if (json.isNull())
    {
        AppEvents::instance()->reportError(
            qApp->tr("Unable to parse shared resources json data: %1").arg(error.errorString()));
        return result;
    }

    static QString sharedDataKey("default");
    static QString remoteServerKey("url");

    auto data = json.object();
    if (!data.contains(sharedDataKey))
    {
        AppEvents::instance()->reportError(
            qApp->tr("Shared resources json data does not contain mandatory key '%1'").arg(sharedDataKey));
        return result;
    }

    auto sharedData = data[sharedDataKey].toObject();
    if (!sharedData.contains(remoteServerKey))
    {
        AppEvents::instance()->reportError(
            qApp->tr("Shared resources json data does not contain mandatory key '%1'").arg(remoteServerKey));
        return result;
    }

    result.url = sharedData[remoteServerKey].toString();
    result.note = sharedData["note"].toString();
    result.weight = int(sharedData["weight"].toDouble());
    return result;
}
