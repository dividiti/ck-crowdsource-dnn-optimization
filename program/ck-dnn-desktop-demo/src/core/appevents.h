#ifndef APPEVENTS_H
#define APPEVENTS_H

#include <QObject>

class AppEvents : public QObject
{
    Q_OBJECT

public:
    static AppEvents* instance();

    static void info(const QString& msg);
    static void warning(const QString& msg);
    static void error(const QString& msg);

signals:
    void onInfo(const QString& msg);
    void onError(const QString& msg);

private:
    AppEvents() {}
};

#endif // APPEVENTS_H

