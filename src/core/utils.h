#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QFile;
class QIODevice;
class QLabel;
class QWidget;
QT_END_NAMESPACE

namespace Utils
{
QByteArray loadTextFromFile(const QString& path);
bool saveTextToFile(const QString& path, const QByteArray &text);
QString makePath(const QStringList &parts);

QString bytesIntoHumanReadable(long bytes);

void moveToDesktopCenter(QWidget* w);

void showTextInfoWindow(const QString& text, bool html, int w, int h);
inline void showTextInfoWindow(const QString& text, int w = 0, int h = 0) { showTextInfoWindow(text, false, w, h); }
inline void showHtmlInfoWindow(const QString& text, int w = 0, int h = 0) { showTextInfoWindow(text, true, w, h); }
void closeAllInfoWindows();

void infoDlg(const QString& text);
bool confirmDlg(const QString& text);
void errorDlg(const QString& text);

QWidget* makeDivider();
QLabel* makeTitle(const QString &title);

QString EOL();

char* makeLocalStr(const QString& s);
}

#endif // UTILS_H

