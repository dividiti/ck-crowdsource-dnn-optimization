#include "appmodels.h"

#include <QApplication>
#include <QJsonDocument>

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

//-----------------------------------------------------------------------------

QString PlatformFeatures::parseJson(const QByteArray& text)
{
    json = QJsonObject();

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(text, &error);
    if (doc.isNull())
        return qApp->tr("Unable to parse platform features json data: %1").arg(error.errorString());

    json = doc.object();

    return QString();
}

//-----------------------------------------------------------------------------

QString RecognitionScenarios::parseJson(const QByteArray& text)
{
    static QString scenariosKey("scenarios");

    jsons = QList<QJsonObject>();

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(text, &error);
    if (doc.isNull())
        return qApp->tr("Unable to parse recognition scenarios json data: %1").arg(error.errorString());

    auto jsonRoot = doc.object();
    auto errorCode = int(jsonRoot["return"].toDouble());
    if (errorCode != 0)
        return qApp->tr("Recognition scenarios json data is in error state: return=%1, error: %2")
                .arg(errorCode).arg(jsonRoot["error"].toString());

    if (!jsonRoot.contains(scenariosKey))
        return qApp->tr("Recognition scenarios json data does not contain mandatory key '%1'").arg(scenariosKey);

    auto array = jsonRoot[scenariosKey].toArray();
    for (auto item = array.constBegin(); item != array.constEnd(); item++)
        jsons << (*item).toObject();

    return QString();
}
