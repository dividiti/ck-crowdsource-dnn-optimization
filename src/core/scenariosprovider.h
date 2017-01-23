#ifndef SCENARIOSPROVIDER_H
#define SCENARIOSPROVIDER_H

#include <QObject>
#include <QMap>

QT_BEGIN_NAMESPACE
class QFile;
class QNetworkReply;
QT_END_NAMESPACE

#include "appmodels.h"

class RemoteDataAccess;

class FileDownloadWork
{
public:
    int scenarioIndex;
    int fileIndex;
    RecognitionScenarioFileItem file;

    QString errorTitle() const;
};

class ScenarioDownloadProgress
{
public:
    int totalFiles;
    int loadedFiles = 0;
    QStringList errors;
};

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

    void downloadScenarioFiles(int scenarioIndex);

signals:
    void scenariosReceived(RecognitionScenarios scenarios);
    void scenarioFileDownloaded(int scenarioIndex, int fileIndex, bool success);

private slots:
    void queryRecognitionScenarios_finished();
    void queryDownloadScenarioFile_finished();

private:
    RemoteDataAccess* _network;
    RecognitionScenarios _current;
    QMap<QNetworkReply*, FileDownloadWork> _fileDownloads;
    QMap<int, ScenarioDownloadProgress> _scenarioDownloads;
};

#endif // SCENARIOSPROVIDER_H
