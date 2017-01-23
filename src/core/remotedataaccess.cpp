#include "appconfig.h"
#include "appevents.h"
#include "remotedataaccess.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#define HTTP_USER_AGENT "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:50.0) Gecko/20100101 Firefox/50.0"
#define HTTP_CONTENT_URLENCODED "application/x-www-form-urlencoded"

RemoteDataAccess::RemoteDataAccess(QObject *parent) : QObject(parent)
{
    _network = new QNetworkAccessManager(this);
    connect(_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(queryAny_finished(QNetworkReply*)));
}

QNetworkReply* RemoteDataAccess::post(const QString& url, const QByteArray& data)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HTTP_CONTENT_URLENCODED);

    return _network->post(request, data);
}

QNetworkReply* RemoteDataAccess::get(const QString& url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, HTTP_USER_AGENT);

    return _network->get(request);
}

void RemoteDataAccess::querySharedRepoInfo(const QString& url)
{
    AppEvents::info(tr("Query shared repository info from %1").arg(url));

    auto reply = get(url);
    connect(reply, SIGNAL(finished()), this, SLOT(querySharedRepoInfo_finished()));
}

void RemoteDataAccess::querySharedRepoInfo_finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error())
        return AppEvents::error(tr("Unable to load shared resources: %1").arg(reply->errorString()));

    SharedRepoInfo info;
    info.parseJson(reply->readAll());
    if (!info.isEmpty())
        emit sharedRepoInfoReceived(info);
}

void RemoteDataAccess::queryAny_finished(QNetworkReply *reply)
{
    reply->deleteLater();
    emit requestFinished();
}

