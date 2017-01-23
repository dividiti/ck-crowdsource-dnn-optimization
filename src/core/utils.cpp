#include "appevents.h"
#include "utils.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include <QFile>
#include <QMessageBox>
#include <QPalette>
#include <QPointer>
#include <QTextBrowser>

namespace Utils
{

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

QString calcFileMD5(QFile* file)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    if (!hash.addData(file))
    {
        AppEvents::error(qApp->tr("Unknown error when calculating MD5 sum for file %1").arg(file->fileName()));
        return QString();
    }
    return QString::fromUtf8(hash.result());
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

QWidget* makeDivider()
{
    auto divider = new QFrame;
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet(QString("border-top-width: 1px;"
                                   "border-top-style: solid;"
                                   "border-top-color: %1;")
                           .arg(divider->palette().color(QPalette::Dark).name()));
    return divider;
}


} // namespace Utils
