#include "appevents.h"
#include "utils.h"

#include <QApplication>
#include <QJsonObject>
#include <QStringList>
#include <QFile>

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
            jsonObjectToList(value.toObject(), report, level+1);
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

QByteArray loadTtextFromFile(const QString& path)
{
    QFile file(path);
    if (!file.exists())
    {
        AppEvents::warning(qApp->tr("File not found: %1").arg(path));
        return QByteArray();
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        AppEvents::error(qApp->tr("Unable to open file %1: %2").arg(path).arg(file.errorString()));
        return QByteArray();
    }

    return file.readAll();
}

void saveTextToFile(const QString& path, const QByteArray text)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return AppEvents::error(qApp->tr("Unable to create file %1: %2").arg(path).arg(file.errorString()));

    if (file.write(text) == -1)
        return AppEvents::error(qApp->tr("Fail writing into file %1: %2").arg(path).arg(file.errorString()));
}

} // namespace Utils
