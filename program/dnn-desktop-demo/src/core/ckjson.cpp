#include "appconfig.h"
#include "ckjson.h"
#include "utils.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

void CkJson::open(const QString& file)
{
    if (!QFile(file).exists())
    {
        qWarning() << "File not found:" << file;
        return;
    }
    auto doc = QJsonDocument::fromJson(Utils::loadTextFromFile(file));
    _json = doc.object();
    if (_json.isEmpty())
    {
        qWarning() << "Invalid CK-json file:" << file;
        return;
    }
    _ok = true;
}

void CkJson::errorKeyNotFound(const QString& key) const
{
    qWarning() << "Required key not found:" << key;
}

void CkJson::warnKeyIsEmpty(const QString& key) const
{
    qWarning() << key << "is empty";
}

QString CkJson::valueStr(const QString& key) const
{
    return valueStr(_json, key);
}

QString CkJson::valueStr(QStringList path) const
{
    if (path.size() == 1)
        return valueStr(_json, path.first());
    auto json = _json;
    for (int i = 0; i < path.size()-1; i++)
    {
        if (!json.contains(path[i]))
        {
            errorKeyNotFound(path.join("/"));
            return QString();
        }
        json = json[path.at(i)].toObject();
    }
    auto key = path.last();
    if (!json.contains(key))
    {
        errorKeyNotFound(path.join("/"));
        return QString();
    }
    return valueStr(json, key);
}

QString CkJson::valueStr(const QJsonObject& json, const QString& key) const
{
    if (!json.contains(key))
    {
        errorKeyNotFound(key);
        return QString();
    }
    auto res = json[key].toString();
    if (res.isEmpty())
        warnKeyIsEmpty(key);
    return res;
}

//-----------------------------------------------------------------------------

CkEnvMeta::CkEnvMeta(const QString& uid)
{
    open(Utils::makePath({ AppConfig::ckReposPath(), "local", "env", uid, ".cm", "meta.json" }));
}

QString CkEnvMeta::envVar(const QString& name) const
{
    return valueStr({"env", name});
}

QStringList CkEnvMeta::tags() const
{
    auto json = _json["tags"].toArray();
    QStringList tags;
    for (auto it = json.constBegin(); it != json.constEnd(); it++)
        tags << (*it).toString();
    return tags;
}

QVector<QPair<QString, QString> > CkEnvMeta::setupEnvs() const
{
    QVector<QPair<QString, QString> > res;
    const QString key("setup");
    if (!_json.contains(key))
    {
        errorKeyNotFound(key);
        return res;
    }
    auto setupObj = _json[key].toObject();
    for (const QString& key: setupObj.keys())
        res << QPair<QString, QString>(key, setupObj[key].toString());
    return res;
}

//-----------------------------------------------------------------------------

CkInfo::CkInfo(const QString& path)
{
    open(Utils::makePath({path, ".cm", "info.json" }));
}

//-----------------------------------------------------------------------------

CkEnvInfo::CkEnvInfo(const QString& uid) : CkInfo(
    Utils::makePath({ AppConfig::ckReposPath(), "local", "env", uid }))
{
}

