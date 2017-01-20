#ifndef SCENARIOSPROVIDER_H
#define SCENARIOSPROVIDER_H

#include <QObject>

#include "appmodels.h"

class RemoteDataAccess;

class ScenariosProvider : public QObject
{
    Q_OBJECT

public:
    explicit ScenariosProvider(RemoteDataAccess *network, QObject *parent = 0);

    void queryScenarios(const QString &url, const PlatformFeatures& features);

signals:
    void scenariosReceived(RecognitionScenarios scenarios);

private slots:
    void queryRecognitionScenarios_finished();

private:
    RemoteDataAccess* _network;
};

#endif // SCENARIOSPROVIDER_H
