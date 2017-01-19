#ifndef UTILS_H
#define UTILS_H

#include <QString>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace Utils
{
QString jsonObjectToString(const QJsonObject& obj);
}

#endif // UTILS_H
