#include "jsonformat.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

QString formatJsonValueAsText(const QString& value)
{
    return value;
}

QString formatJsonValueAsHtml(const QString& value)
{
    return QString("<b>%1</b>").arg(value);
}


JsonFormat::JsonFormat(Format format)
{
    if (format == Text)
    {
        _lineDelimiter = "\n";
        _indent = "    ";
        formatValue = formatJsonValueAsText;
    }
    else
    {
        _lineDelimiter = "<br>";
        _indent = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        formatValue = formatJsonValueAsHtml;
    }
}

QString JsonFormat::jsonValueToString(const QJsonValue& value)
{
    if (value.isBool())
        return value.toBool()? "true": "false";
    if (value.isDouble())
        return QString::number(value.toDouble());
    if (value.isString())
        return value.toString();
    if (value.isNull())
        return "(null)";
    if (value.isUndefined())
        return "(undefined)";
    return QString();
}

void JsonFormat::jsonArrayToList(const QJsonArray& arr, QStringList& report, int level)
{
    int index = 0;
    for (auto item = arr.constBegin(); item != arr.constEnd(); item++)
        jsonValueToList(QString("[%1]").arg(index++), *item, report, level+1);
}

void JsonFormat::jsonObjectToList(const QJsonObject& obj, QStringList& report, int level)
{
    for (auto item = obj.constBegin(); item != obj.constEnd(); item++)
        jsonValueToList(item.key(), item.value(), report, level);
}

void JsonFormat::jsonValueToList(const QString &key, const QJsonValue& value, QStringList& report, int level)
{
    QString intend;
    for (int i = 0; i < level; i++)
        intend += _indent;

    if (value.isObject())
    {
        report << QString("%1%2: ").arg(intend, key);
        jsonObjectToList(value.toObject(), report, level+1);
    }
    else if (value.isArray())
    {
        report << QString("%1%2: ").arg(intend, key);
        jsonArrayToList(value.toArray(), report, level+1);
    }
    else
        report << QString("%1%2: %3").arg(intend, key, formatValue(jsonValueToString(value)));
}

QString JsonFormat::format(const QJsonObject& obj)
{
    QStringList report;
    jsonObjectToList(obj, report, 0);
    return report.join(_lineDelimiter);
}
