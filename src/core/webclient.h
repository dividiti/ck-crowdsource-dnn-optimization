#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include <QObject>

class WebResult
{
public:
    static WebResult success(const QString data) { return WebResult(true, data); }
    static WebResult fail(const QString error) { return WebResult(false, error); }

    bool ok() const { return _ok; }
    bool failed() const { return !_ok; }
    QString error() const { return _ok? QString(): _data; }
    QString data() const { return _ok? _data: QString(); }

private:
    WebResult(bool ok, const QString& data) { _ok = ok, _data = data; }

    bool _ok;
    QString _data;
};

//-----------------------------------------------------------------------------

class WebClient : public QObject
{
    Q_OBJECT

public:
    explicit WebClient(QObject *parent = 0) : QObject(parent) {}

    static WebResult loadStringData(const QString& url);
};

#endif // WEBCLIENT_H
