#include "webclient.h"

#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace InternalWebClient
{
    QNetworkAccessManager* networkAccess()
    {
        static QNetworkAccessManager network;
        return &network;
    }

    QNetworkReply* get(const QString& url)
    {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader,
            "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:50.0) Gecko/20100101 Firefox/50.0");

        return networkAccess()->get(request);
    }

} // namespace InternalWebClient

//-----------------------------------------------------------------------------

WebResult WebClient::loadStringData(const QString& url)
{
    QScopedPointer<QNetworkReply> reply(InternalWebClient::get(url));
    if (!reply->isFinished())
    {
        QEventLoop waitForGet;
        connect(reply.data(), &QNetworkReply::finished, &waitForGet, &QEventLoop::quit);
        waitForGet.exec();
    }
    return reply->error()
            ? WebResult::fail(reply->errorString())
            : WebResult::success(reply->readAll());
}
