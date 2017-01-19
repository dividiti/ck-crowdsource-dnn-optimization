#ifndef REMOTEDATAACCESS_H
#define REMOTEDATAACCESS_H

#include "appmodels.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class RemoteDataAccess : public QObject
{
    Q_OBJECT

public:
    explicit RemoteDataAccess(QObject *parent = 0);

    void querySharedRepoInfo(const QString& url);
    void queryScenarios(const QString &url, const PlatformFeatures& features);

signals:
    void requestFinished();
    void sharedRepoInfoReceived(SharedRepoInfo info);
    void scenariosReceived(RecognitionScenarios scenarios);

private slots:
    void querySharedRepoInfo_finished();
    void queryRecognitionScenarios_finished();
    void queryAny_finished(QNetworkReply *reply);

private:
    QNetworkAccessManager* _network;
};

#endif // REMOTEDATAACCESS_H
