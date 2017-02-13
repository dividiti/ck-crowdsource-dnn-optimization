#include "appevents.h"
#include "utils.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QPointer>
#include <QTextBrowser>

namespace Utils
{

QByteArray loadTextFromFile(const QString& path)
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

bool saveTextToFile(const QString& path, const QByteArray& text)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        AppEvents::error(qApp->tr("Unable to create file %1: %2").arg(path).arg(file.errorString()));
        return false;
    }
    if (file.write(text) == -1)
    {
        AppEvents::error(qApp->tr("Fail writing into file %1: %2").arg(path).arg(file.errorString()));
        return false;
    }
    return true;
}

QString makePath(const QStringList &parts)
{
    return parts.join(QDir::separator());
}

QString bytesIntoHumanReadable(long bytes)
{
    const double kilobyte = 1000;
    const double megabyte = kilobyte * 1000;
    const double gigabyte = megabyte * 1000;
    const double terabyte = gigabyte * 1000;

    if ((bytes >= 0) && (bytes < kilobyte))
        return qApp->tr("%1 B").arg(bytes);

    if ((bytes >= kilobyte) && (bytes < megabyte))
        return qApp->tr("%1 KB").arg(qRound(bytes / kilobyte));

    if ((bytes >= megabyte) && (bytes < gigabyte))
        return qApp->tr("%1 MB").arg(qRound(bytes / megabyte));

    if ((bytes >= gigabyte) && (bytes < terabyte))
        return qApp->tr("%1 GB").arg(qRound(bytes / gigabyte));

    if (bytes >= terabyte)
        return qApp->tr("%1 TB").arg(qRound(bytes / terabyte));

    return qApp->tr("%1 Bytes").arg(bytes);
}

void moveToDesktopCenter(QWidget* w)
{
    auto desktop = QApplication::desktop()->availableGeometry(w);
    w->move(desktop.center() - w->rect().center());
}

namespace UtilsInfoWidows {
    // rememeber all opened windows and close them when main window is closed
    QList<QPointer<QWidget>> windows;
}

void showTextInfoWindow(const QString& text, bool html, int w, int h)
{
    auto window = new QTextBrowser;
    window->setAttribute(Qt::WA_DeleteOnClose);
    if (html)
        window->setHtml(text);
    else
        window->setPlainText(text);
    auto f = window->font();
    f.setPointSize(11);
    window->setFont(f);
    if (w > 0 && h > 0)
        window->resize(w, h);
    moveToDesktopCenter(window);
    window->show();
    UtilsInfoWidows::windows.append(window);
}

void closeAllInfoWindows()
{
    for (auto w: UtilsInfoWidows::windows)
        if (w) delete w;
    UtilsInfoWidows::windows.clear();
}

void infoDlg(const QString& text)
{
    QMessageBox::information(qApp->activeWindow(), qApp->activeWindow()->windowTitle(), text);
}

bool confirmDlg(const QString& text)
{
    return QMessageBox::question(qApp->activeWindow(), qApp->activeWindow()->windowTitle(), text,
                                 QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok;
}

void errorDlg(const QString& text)
{
    QMessageBox::critical(qApp->activeWindow(), qApp->activeWindow()->windowTitle(), text);
}

QWidget* makeDivider()
{
    auto divider = new QFrame;
    divider->setProperty("qss-role", "divider");
    divider->setFrameShape(QFrame::HLine);
    return divider;
}

QLabel* makeTitle(const QString& title)
{
    auto label = new QLabel(title);
    label->setProperty("qss-role", "panel-title");
    auto f = label->font();
    f.setLetterSpacing(QFont::AbsoluteSpacing, 0.8);
    label->setFont(f);
    return label;
}

QString EOL()
{
#ifdef Q_OS_WIN
    return QStringLiteral("\r\n");
#endif
#ifdef Q_OS_MAC
    return QStringLiteral("\r");
#endif
    return QStringLiteral("\n");
}

} // namespace Utils
