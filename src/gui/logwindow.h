#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QPlainTextEdit>

class LogWindow : public QPlainTextEdit
{
    Q_OBJECT
public:
    static LogWindow* instance();
    static void destroyInstance();
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);
private:
    static QPointer<LogWindow> _instance;
    explicit LogWindow(QWidget *parent = 0);
    static QString sanitizeHtml(const QString& msg);
};

#endif // LOGWINDOW_H
