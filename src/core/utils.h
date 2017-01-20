#ifndef UTILS_H
#define UTILS_H

#include <QString>

QT_BEGIN_NAMESPACE
class QFile;
class QJsonObject;
class QWidget;
QT_END_NAMESPACE

namespace Utils
{
QString jsonObjectToString(const QJsonObject& obj);

QByteArray loadTtextFromFile(const QString& path);
void saveTextToFile(const QString& path, const QByteArray text);
QString calcFileMD5(QFile* file);

QString bytesIntoHumanReadable(long bytes);

void moveToDesktopCenter(QWidget* w);
void showTextInfoWindow(const QString& text, int w = 0, int h = 0);

void infoDlg(const QString& text);
}

#endif // UTILS_H
