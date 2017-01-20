#include "appconfig.h"
#include "appevents.h"
#include "appmodels.h"
#include "utils.h"

#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>

#define KEY_SCENARIOS "scenarios"
#define KEY_RETURN "return"
#define KEY_ERROR "error"

void SharedRepoInfo::parseJson(const QByteArray& text)
{
    static QString repoKey("default");
    static QString urlKey("url");

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(text, &error);
    if (doc.isNull())
        return AppEvents::error(qApp->tr("Unable to parse shared resources json data: %1").arg(error.errorString()));

    auto json = doc.object();
    if (!json.contains(repoKey))
        return AppEvents::error(qApp->tr("Shared resources json data does not contain mandatory key '%1'").arg(repoKey));

    auto info = json[repoKey].toObject();
    if (!info.contains(urlKey))
        return AppEvents::error(qApp->tr("Shared resources json data does not contain mandatory key '%1'").arg(urlKey));

    _url = info[urlKey].toString();
    _note = info["note"].toString();
    _weight = int(info["weight"].toDouble());
}

void SharedRepoInfo::loadFromConfig()
{
    _url = AppConfig::sharedRepoUrl();
}

void SharedRepoInfo::saveToConfig()
{
    AppConfig::setSharedRepoUrl(_url);
}

QString SharedRepoInfo::str() const
{
    return QString("url: %1; weight: %2; note: %3").arg(_url).arg(_weight).arg(_note);
}

//-----------------------------------------------------------------------------

void PlatformFeatures::parseJson(const QByteArray& text)
{
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(text, &error);
    if (doc.isNull())
        return AppEvents::error(qApp->tr("Unable to parse platform features json data: %1").arg(error.errorString()));

    _json = doc.object();

    auto os = _json["os"].toObject();
    _osName = os["name"].toString();
}

void PlatformFeatures::loadFromFile(const QString& path)
{
    auto text = Utils::loadTtextFromFile(path);
    if (!text.isEmpty())
        parseJson(text);
}

void PlatformFeatures::saveToFile(const QString& path) const
{
    Utils::saveTextToFile(path, QJsonDocument(_json).toJson());
}

QString PlatformFeatures::str() const
{
    return Utils::jsonObjectToString(_json);
}

//-----------------------------------------------------------------------------

bool RecognitionScenarioFileItem::parseJson(const QJsonObject& json)
{
    _name = json["filename"].toString();
    _path = json["path"].toString();
    _md5 = json["md5"].toString();
    _url = json["url"].toString();
    return true;
}

QString RecognitionScenarioFileItem::fullPath() const
{
    return AppConfig::scenariosDataDir() + _path + QDir::separator() + _name;
}

bool RecognitionScenarioFileItem::isLoaded() const
{
    QFile f(fullPath());
    qDebug() << "Check MD5 for" << f.fileName() << _md5;
    if (!f.exists())
    {
        qDebug() << "File not found";
        return false;
    }
    auto md5 = Utils::calcFileMD5(&f);
    qDebug() << md5;
    if (md5.compare(_md5, Qt::CaseInsensitive) != 0)
    {
        qDebug() << "FAIL";
        return false;
    }
    qDebug() << "OK";
    return true;
}

//-----------------------------------------------------------------------------

bool RecognitionScenario::parseJson(const QJsonObject& json)
{
    _json = json;

    // TODO: default value will be used if some key not found, should we check if key exists?

    _fileSizeBytes = long(json["total_file_size"].toDouble());
    _fileSizeMB = Utils::bytesIntoHumanReadable(_fileSizeBytes);

    auto meta = json["meta"].toObject();
    _title = meta["title"].toString();

    QJsonArray files = meta["files"].toArray();
    for (auto file = files.constBegin(); file != files.constEnd(); file++)
    {
        RecognitionScenarioFileItem fileItem;
        if (fileItem.parseJson((*file).toObject()))
            _files.append(fileItem);
    }

    return true;
}

QString RecognitionScenario::str() const
{
    return Utils::jsonObjectToString(_json);
}

bool RecognitionScenario::allFilesAreLoaded() const
{
    for (const RecognitionScenarioFileItem& file: _files)
        if (!file.isLoaded())
            return false;
    return true;
}

//-----------------------------------------------------------------------------

void RecognitionScenarios::parseJson(const QByteArray& text)
{
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(text, &error);
    if (doc.isNull())
        return AppEvents::error(qApp->tr("Unable to parse recognition scenarios json data: %1").arg(error.errorString()));

    auto jsonRoot = doc.object();
    auto errorCode = int(jsonRoot[KEY_RETURN].toDouble());
    if (errorCode != 0)
        return AppEvents::error(qApp->tr("Recognition scenarios json data is in error state: return=%1, error: %2")
                .arg(errorCode).arg(jsonRoot[KEY_ERROR].toString()));

    if (!jsonRoot.contains(KEY_SCENARIOS))
        return AppEvents::error(qApp->tr("Recognition scenarios json data does not contain mandatory key '%1'").arg(KEY_SCENARIOS));

    auto array = jsonRoot[KEY_SCENARIOS].toArray();
    for (auto item = array.constBegin(); item != array.constEnd(); item++)
    {
        RecognitionScenario scenario;
        if (scenario.parseJson((*item).toObject()))
            _items << scenario;
    }
}

void RecognitionScenarios::loadFromFile(const QString& path)
{
    auto text = Utils::loadTtextFromFile(path);
    if (!text.isEmpty())
        parseJson(text);
}

void RecognitionScenarios::saveToFile(const QString& path) const
{
    QJsonArray array;
    for (const RecognitionScenario& item: _items)
        array.append(item.json());

    QJsonObject json;
    json[KEY_SCENARIOS] = array;

    Utils::saveTextToFile(path, QJsonDocument(json).toJson());
}

QString RecognitionScenarios::str() const
{
    QStringList report;
    report << QString("Count: %1").arg(_items.size());
    for (const RecognitionScenario& item: _items)
    {
        report << "-----------------------------";
        report << item.str();
    }
    return report.join("\n");
}
