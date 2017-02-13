#include "logwindow.h"

#include <QPointer>

LogWindow::LogWindow(QWidget *parent) : QTextEdit(parent)
{
#ifdef Q_OS_WIN
    setFont(QFont("Courier", 9));
#else
    setFont(QFont("Monospace", 10));
#endif
    setGeometry(10, 30, 800, 500);
}

QPointer<LogWindow> LogWindow::_instance;

LogWindow* LogWindow::instance()
{
    if (!_instance)
        _instance = new LogWindow;
    return _instance;
}

void LogWindow::destroyInstance()
{
    if (_instance)
        delete _instance;
}

void LogWindow::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QString msg = QString(QStringLiteral("<p><b>%1</b>: %2"))
            .arg(messageType(type), sanitizeHtml(message));

    if (context.file)
        msg += QString(QStringLiteral("<br><font color=gray>(%2:%2, %4)</font>"))
            .arg(context.file).arg(context.line).arg(context.function);

    auto w = instance();
    w->append(msg);
    if (!w->isVisible())
        w->show();
}

QString LogWindow::sanitizeHtml(const QString& msg)
{
    return QString(msg).replace("<", "&lt;").replace(">", "&gt;").replace("\n", "<br>");
}

QString LogWindow::messageType(QtMsgType type)
{
    switch (type)
    {
    case QtDebugMsg: return QStringLiteral("DEBUG");
    case QtWarningMsg: return QStringLiteral("WARNING");
    case QtCriticalMsg: return QStringLiteral("CRITICAL");
    case QtFatalMsg: return QStringLiteral("FATAL");
    default: return QString();
    }
}
