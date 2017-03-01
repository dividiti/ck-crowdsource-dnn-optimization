#ifndef JSONFORMAT_H
#define JSONFORMAT_H

#include <QString>

QT_BEGIN_NAMESPACE
class QJsonArray;
class QJsonObject;
class QJsonValue;
class QStringList;
QT_END_NAMESPACE

class JsonFormat
{
public:
    enum Format { Text, Html };

    JsonFormat(Format format);

    QString format(const QJsonObject& obj);

private:
    QString _lineDelimiter, _indent;
    QString (*formatValue)(const QString& value);

    void jsonValueToList(const QString &key, const QJsonValue& value, QStringList& report, int level);
    void jsonObjectToList(const QJsonObject& obj, QStringList& report, int level);
    void jsonArrayToList(const QJsonArray& arr, QStringList& report, int level);
    QString jsonValueToString(const QJsonValue& value);
};

#endif // JSONFORMAT_H

