#ifndef PLATFORMFEATURESPROVIDER_H
#define PLATFORMFEATURESPROVIDER_H

#include "appmodels.h"

#include <QObject>

class PlatformFeaturesProvider : public QObject
{
    Q_OBJECT

public:
    explicit PlatformFeaturesProvider(QObject *parent = 0) : QObject(parent) {}

    void queryPlatformFeatures(const QString &sharedRepoUrl);

signals:
    void platformFeaturesReceived(PlatformFeatures features);
};

#endif // PLATFORMFEATURESPROVIDER_H
