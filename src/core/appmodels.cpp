#include "appmodels.h"

#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>

QString SharedRepoInfo::parseJson(const QByteArray& text)
{
    static QString repoKey("default");
    static QString urlKey("url");

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(text, &error);
    if (doc.isNull())
        return qApp->tr("Unable to parse shared resources json data: %1").arg(error.errorString());

    auto json = doc.object();
    if (!json.contains(repoKey))
        return qApp->tr("Shared resources json data does not contain mandatory key '%1'").arg(repoKey);

    auto info = json[repoKey].toObject();
    if (!info.contains(urlKey))
        return qApp->tr("Shared resources json data does not contain mandatory key '%1'").arg(urlKey);

    _url = info[urlKey].toString();
    _note = info["note"].toString();
    _weight = int(info["weight"].toDouble());

    return QString();
}
