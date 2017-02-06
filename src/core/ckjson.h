#ifndef CKJSON_H
#define CKJSON_H

#include <QJsonObject>
#include <QStringList>

class CkJson
{
public:
    const QJsonObject& json() const { return _json; }
    bool ok() const { return _ok; }

    QStringList subkeys(const QString& key) const;
    QString valueStr(const QString& key) const;
    QString valueStr(QStringList path) const;

protected:
    QJsonObject _json;

    void open(const QString& fileName);
    void errorKeyNotFound(const QString& key) const;
    void warnKeyIsEmpty(const QString& key) const;

private:
    bool _ok = false;

    QString valueStr(const QJsonObject& json, const QString& key) const;
};

//-----------------------------------------------------------------------------

class CkEnvMeta : public CkJson
{
public:
    CkEnvMeta(const QString& uid);

    QString envVar(const QString& name) const;
    QStringList tags() const;
    QString packageUoa() const { return valueStr("package_uoa"); }
    QString fullPath() const { return valueStr({"customize", "full_path"}); }
    QString pathLib() const { return valueStr({"customize", "path_lib"}); }
    QString dynamicLib() const { return valueStr({"customize", "dynamic_lib"}); }
    QVector<QPair<QString, QString> > setupEnvs() const;
};

//-----------------------------------------------------------------------------

class CkInfo : public CkJson
{
public:
    CkInfo(const QString& path);

    QString dataName() const { return valueStr("data_name"); }
};

#endif // CKJSON_H
