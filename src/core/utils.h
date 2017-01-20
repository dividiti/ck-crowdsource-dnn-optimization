#ifndef UTILS_H
#define UTILS_H

#include <QString>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace Utils
{
QString jsonObjectToString(const QJsonObject& obj);

QByteArray loadTtextFromFile(const QString& path);
void saveTextToFile(const QString& path, const QByteArray text);

QString bytesIntoHumanReadable(long bytes);
}

#endif // UTILS_H
