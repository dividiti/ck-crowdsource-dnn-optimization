#ifndef APPEVENTS_H
#define APPEVENTS_H

#include <QObject>

class AppEvents : public QObject
{
    Q_OBJECT

public:
    static AppEvents* instance();

    void reportError(const QString& msg);

signals:
    void error(const QString& msg);

private:
    AppEvents() {}
};

#endif // APPEVENTS_H
