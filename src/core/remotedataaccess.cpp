#include "appevents.h"
#include "remotedataaccess.h"

#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

RemoteDataAccess::RemoteDataAccess(QObject *parent) : QObject(parent)
{
    _network = new QNetworkAccessManager(this);
    connect(_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(queryAny_finished(QNetworkReply*)));
}

void RemoteDataAccess::querySharedRepoInfo(const QString& url)
{
    auto reply = httpGet(url);
    connect(reply, SIGNAL(finished()), this, SLOT(querySharedRepoInfo_finished()));
}

void RemoteDataAccess::querySharedRepoInfo_finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error())
        return AppEvents::instance()->reportError(
            qApp->tr("Unable to load shared resources: %1").arg(reply->errorString()));

    SharedRepoInfo repoInfo;
    auto res = repoInfo.parseJson(reply->readAll());
    if (!res.isEmpty())
        return AppEvents::instance()->reportError(res);

    emit sharedRepoInfoAqcuired(repoInfo);
}

void RemoteDataAccess::queryAny_finished(QNetworkReply *reply)
{
    reply->deleteLater();
    emit requestFinished();
}

QNetworkReply* RemoteDataAccess::httpGet(const QString& url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
        "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:50.0) Gecko/20100101 Firefox/50.0");

    return _network->get(request);
}
