#include "appconfig.h"
#include "appevents.h"
#include "remotedataaccess.h"

#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
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

void RemoteDataAccess::querySharedRepoInfo(const QString& url)
{
    AppEvents::info(tr("Query shared repository info from %1").arg(url));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, HTTP_USER_AGENT);

    auto reply = _network->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(querySharedRepoInfo_finished()));
}

void RemoteDataAccess::queryScenarios(const QString& url, const PlatformFeatures& features)
{
    AppEvents::info(tr("Query recognition scenarios from %1").arg(url));

    QJsonObject json;
    json["action"] = "get";
    json["module_uoa"] = "experiment.scenario.mobile"; // TODO: which module?
    json["platform_features"] = features.json;
    auto postData = "ck_json=" + QUrl::toPercentEncoding(QJsonDocument(json).toJson());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HTTP_CONTENT_URLENCODED);

    auto reply = _network->post(request, postData);
    connect(reply, SIGNAL(finished()), this, SLOT(queryRecognitionScenarios_finished()));
}

void RemoteDataAccess::querySharedRepoInfo_finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error())
        return AppEvents::error(qApp->tr("Unable to load shared resources: %1").arg(reply->errorString()));

    SharedRepoInfo info;
    info.parseJson(reply->readAll());
    if (!info.isEmpty())
        emit sharedRepoInfoReceived(info);
}

void RemoteDataAccess::queryRecognitionScenarios_finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error())
        return AppEvents::error(qApp->tr("Unable to load recognition scenarios list: %1").arg(reply->errorString()));

    RecognitionScenarios scenarios;
    scenarios.parseJson(reply->readAll());
    if (!scenarios.isEmpty())
        emit scenariosReceived(scenarios);
}

void RemoteDataAccess::queryAny_finished(QNetworkReply *reply)
{
    reply->deleteLater();
    emit requestFinished();
}

