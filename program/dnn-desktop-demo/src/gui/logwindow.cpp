#include "logwindow.h"

#include <QPointer>

LogWindow::LogWindow(QWidget *parent) : QPlainTextEdit(parent)
{
#ifdef Q_OS_WIN
    setFont(QFont("Courier", 9));
#else
    setFont(QFont("Monospace", 10));
#endif
    setGeometry(10, 30, 800, 500);
    setReadOnly(true);
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
    Q_UNUSED(type)
    Q_UNUSED(context)
    auto w = instance();
    w->appendPlainText(message);
    if (!w->isVisible())
        w->show();
}

