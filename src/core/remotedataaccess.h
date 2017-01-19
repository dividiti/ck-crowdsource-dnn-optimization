#ifndef REMOTEDATAACCESS_H
#define REMOTEDATAACCESS_H

#include "appmodels.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QNetworkReply;
QT_END_NAMESPACE

class RemoteDataAccess : public QObject
{
    Q_OBJECT

public:
    explicit RemoteDataAccess(QObject *parent = 0);

    void querySharedRepoInfo(const QString& url);

signals:
    void requestFinished();
    void sharedRepoInfoAqcuired(SharedRepoInfo info);
    void platformFeaturesAqcuired();
    void recognitionScenariosAqcuired();

private slots:
    void querySharedRepoInfo_finished();
    void queryAny_finished(QNetworkReply *reply);
};

#endif // REMOTEDATAACCESS_H
