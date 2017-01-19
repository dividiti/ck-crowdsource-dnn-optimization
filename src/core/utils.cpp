#include "utils.h"

#include <QJsonObject>
#include <QStringList>

namespace Utils
{

QString jsonValueToString(const QJsonValue& value)
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
    if (value.isArray())
        return "(array)";
    return QString();
}

void jsonObjectToList(const QJsonObject& obj, QStringList& report, int level)
{
    QString intend;
    intend.fill(' ', level*4);

    for (auto item = obj.constBegin(); item != obj.constEnd(); item++)
    {
        auto key = item.key();
        auto value = item.value();
        if (value.isObject())
        {
            report << QString("%1%2: ").arg(intend, key);
            jsonObjectToList(value.toObject(), report, ++level);
        }
        else
            report << QString("%1%2: %3").arg(intend, key, jsonValueToString(value));
    }
}

QString jsonObjectToString(const QJsonObject& obj)
{
    QStringList report;
    jsonObjectToList(obj, report, 0);
    return report.join("\n");
}

} // namespace Utils
