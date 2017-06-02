#ifndef APPEVENTS_H
#define APPEVENTS_H

#include <QObject>

class AppEvents : public QObject
{
    Q_OBJECT

public:
    static AppEvents* instance();

    // Must be called at application startup
    void init();

    static void info(const QString& msg);
    static void warning(const QString& msg);
    static void error(const QString& msg);

public slots:
    void killChildProcesses();

signals:
    void onInfo(const QString& msg);
    void onError(const QString& msg);

private:
    AppEvents() {}
};

#endif // APPEVENTS_H

