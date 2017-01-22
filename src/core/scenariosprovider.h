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

    RecognitionScenarios loadFromCache();
    void saveToCahe(const RecognitionScenarios& scenarios) const;

    const RecognitionScenarios& currentList() const { return _current; }
    void setCurrentList(const RecognitionScenarios& scenarios) { _current = scenarios; }

signals:
    void scenariosReceived(RecognitionScenarios scenarios);

private slots:
    void queryRecognitionScenarios_finished();

private:
    RemoteDataAccess* _network;
    RecognitionScenarios _current;
};

#endif // SCENARIOSPROVIDER_H
