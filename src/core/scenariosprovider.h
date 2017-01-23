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

    QString title() const;
};

class ScenarioDownloadStatus
{
public:
    enum Status { NoFiles, AllLoaded, IsLoading };

    Status status() const { return _status; }
    int totalFiles() const { return _totalFiles; }
    int loadedFiles() const { return _loadedFiles; }
    QString error() const { return _errors.join("\n"); }
    bool ok() const { return _errors.isEmpty(); }

private:
    int _totalFiles;
    int _loadedFiles;
    QStringList _errors;
    Status _status;

    friend class ScenariosProvider;
};

class ScenariosProvider : public QObject
{
    Q_OBJECT

public:
    explicit ScenariosProvider(RemoteDataAccess *network, QObject *parent = 0);
    ~ScenariosProvider();

    void queryScenarios(const QString &url, const PlatformFeatures& features);

    RecognitionScenarios loadFromCache();
    void saveToCahe(const RecognitionScenarios& scenarios) const;

    const RecognitionScenarios& currentList() const { return _current; }
    void setCurrentList(const RecognitionScenarios& scenarios);

    ScenarioDownloadStatus* scenarioDownloadStatus(int scenarioIndex);
    void downloadScenarioFiles(int scenarioIndex);
    void deleteScenarioFiles(int scenarioIndex);

signals:
    void scenariosReceived(RecognitionScenarios scenarios);
    void scenarioFileDownloaded(int scenarioIndex, int loadedFilesCount);
    void filesDownloadComplete(int scenarioIndex, const QString& errors);

private slots:
    void queryRecognitionScenarios_finished();
    void queryDownloadScenarioFile_finished();

private:
    RemoteDataAccess* _network;
    RecognitionScenarios _current;
    QMap<QNetworkReply*, FileDownloadWork> _fileDownloads;
    QMap<int, ScenarioDownloadStatus*> _scenarioDownloads;

    void clearDownloadStatuses();
    void prepareDownloadStatuses();

    void processDownloadProgress(QNetworkReply *reply, const FileDownloadWork &download, QString error);
};

#endif // SCENARIOSPROVIDER_H
