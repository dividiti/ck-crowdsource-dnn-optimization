#ifndef CKJSON_H
#define CKJSON_H

#include <QJsonObject>

class CkJson
{
public:
    bool ok() const { return _ok; }

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
};

//-----------------------------------------------------------------------------

class CkInfo : public CkJson
{
public:
    CkInfo(const QString& path);

    QString dataName() const { return valueStr("data_name"); }
};

#endif // CKJSON_H
