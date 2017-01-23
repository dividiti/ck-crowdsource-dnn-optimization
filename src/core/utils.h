#ifndef UTILS_H
#define UTILS_H

#include <QString>

QT_BEGIN_NAMESPACE
class QFile;
class QWidget;
QT_END_NAMESPACE

namespace Utils
{
QByteArray loadTtextFromFile(const QString& path);
void saveTextToFile(const QString& path, const QByteArray text);
QString calcFileMD5(QFile* file);

QString bytesIntoHumanReadable(long bytes);

void moveToDesktopCenter(QWidget* w);

void showTextInfoWindow(const QString& text, bool html, int w, int h);
inline void showTextInfoWindow(const QString& text, int w = 0, int h = 0) { showTextInfoWindow(text, false, w, h); }
inline void showHtmlInfoWindow(const QString& text, int w = 0, int h = 0) { showTextInfoWindow(text, true, w, h); }
void closeAllInfoWindows();

void infoDlg(const QString& text);

QWidget* makeDivider();
}

#endif // UTILS_H

