#include "appconfig.h"
#include "appevents.h"
#include "remotedataaccess.h"
#include "scenariosprovider.h"
#include "utils.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrl>

QString FileDownloadWork::title() const
{
    return qApp->tr("Downloading file '%1' from %2 into %3").arg(file.name(), file.url(), file.fullPath());
}

//-----------------------------------------------------------------------------

ScenariosProvider::ScenariosProvider(RemoteDataAccess *network, QObject *parent) : QObject(parent), _network(network)
{
}

ScenariosProvider::~ScenariosProvider()
{
    clearDownloadStatuses();
}

void ScenariosProvider::queryScenarios(const QString& url, const PlatformFeatures& features)
{
    AppEvents::info(tr("Query recognition scenarios from %1").arg(url));

    QJsonObject json;
    json["action"] = "get";
    json["module_uoa"] = "experiment.scenario.mobile"; // TODO: which module?
    json["platform_features"] = features.json();
    auto postData = "ck_json=" + QUrl::toPercentEncoding(QJsonDocument(json).toJson());

    auto reply = _network->post(url, postData);
    connect(reply, SIGNAL(finished()), this, SLOT(queryRecognitionScenarios_finished()));
}

void ScenariosProvider::queryRecognitionScenarios_finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error())
        return AppEvents::error(tr("Unable to load recognition scenarios list: %1").arg(reply->errorString()));

    RecognitionScenarios scenarios;
    scenarios.parseJson(reply->readAll());
    if (!scenarios.isEmpty())
        emit scenariosReceived(scenarios);
}

RecognitionScenarios ScenariosProvider::loadFromCache()
{
    RecognitionScenarios scenarios;
    scenarios.loadFromFile(AppConfig::scenariosCacheFile());
    return scenarios;
}

void ScenariosProvider::saveToCahe(const RecognitionScenarios& scenarios) const
{
    scenarios.saveToFile(AppConfig::scenariosCacheFile());
}

void ScenariosProvider::setCurrentList(const RecognitionScenarios& scenarios)
{
    _current = scenarios;

    clearDownloadStatuses();
    prepareDownloadStatuses();
}

void ScenariosProvider::downloadScenarioFiles(int scenarioIndex)
{
    if (scenarioIndex < 0 || scenarioIndex >= _current.items().size())
        return AppEvents::error(tr("Unable to download files for scenario: invalid scenario index %1").arg(scenarioIndex));

    auto status = _scenarioDownloads[scenarioIndex];
    status->_status = ScenarioDownloadStatus::IsLoading;
    status->_loadedFiles = 0;
    status->_errors.clear();

    int fileIndex = 0;
    for (const RecognitionScenarioFileItem& file: _current.items().at(scenarioIndex).files())
    {
        FileDownloadWork download { scenarioIndex, fileIndex, file };
        qDebug() << download.title();
        auto reply = _network->get(file.url()); // start downloading async
        connect(reply, &QNetworkReply::finished, this, &ScenariosProvider::queryDownloadScenarioFile_finished);
        // TODO: connect to void downloadProgress(qint64 bytesReceived, qint64 bytesTotal) and report progress in loaded bytes
        _fileDownloads.insert(reply, download);
        fileIndex++;
    }
}

void ScenariosProvider::queryDownloadScenarioFile_finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !_fileDownloads.contains(reply)) return;

    const FileDownloadWork& download = _fileDownloads[reply];

    if (!QDir().mkpath(download.file.fullPath()))
        return processDownloadProgress(reply, download, tr("Unable to create target directory"));

    if (reply->error())
        return processDownloadProgress(reply, download, reply->errorString());

    QFile file(download.file.fullFileName());
    if (!file.open(QIODevice::WriteOnly))
        return processDownloadProgress(reply, download, tr("Unable to create target file: %1").arg(file.errorString()));

    if (file.write(reply->readAll()) == -1)
        return processDownloadProgress(reply, download, tr("Fail writing file: %2").arg(file.errorString()));

    processDownloadProgress(reply, download, QString());
}

void ScenariosProvider::processDownloadProgress(QNetworkReply* reply, const FileDownloadWork& download, QString error)
{
    auto status = _scenarioDownloads[download.scenarioIndex];
    status->_loadedFiles++;

    if (!error.isEmpty())
    {
        qDebug() << "FAIL" << download.title() << error;
        status->_errors.append(download.title() + ": " + error);
    }
    else
        qDebug() << download.file.name() << "OK";

    emit scenarioFileDownloaded(download.scenarioIndex, status->_loadedFiles);

    _fileDownloads.remove(reply);

    if (status->_loadedFiles == status->_totalFiles)
    {
        if (status->ok())
        {
            status->_status = ScenarioDownloadStatus::AllLoaded;
            qDebug() << "Downloading OK for scenario" << download.scenarioIndex;
        }
        else
        {
            status->_status = ScenarioDownloadStatus::NoFiles;
            qDebug() << "Downloading FAILED for scenario" << download.scenarioIndex;
        }
        emit filesDownloadComplete(download.scenarioIndex, status->error());
    }
}

void ScenariosProvider::deleteScenarioFiles(int scenarioIndex)
{
    // TODO
}

ScenarioDownloadStatus* ScenariosProvider::scenarioDownloadStatus(int scenarioIndex)
{
    return _scenarioDownloads.contains(scenarioIndex)? _scenarioDownloads[scenarioIndex]: nullptr;
}

void ScenariosProvider::clearDownloadStatuses()
{
    for (auto i = _scenarioDownloads.constBegin(); i != _scenarioDownloads.constEnd(); i++)
        if (i.value()) delete i.value();
    _scenarioDownloads.clear();
}

void ScenariosProvider::prepareDownloadStatuses()
{
    for (int i = 0; i < _current.items().size(); i++)
    {
        const RecognitionScenario& scenario = _current.items().at(i);
        auto status = new ScenarioDownloadStatus;
        status->_totalFiles = scenario.files().size();
        if (scenario.allFilesAreLoaded())
        {
            status->_status = ScenarioDownloadStatus::AllLoaded;
            status->_loadedFiles = scenario.files().size();
        }
        else
        {
            status->_status = ScenarioDownloadStatus::NoFiles;
            status->_loadedFiles = 0;
        }
        _scenarioDownloads.insert(i, status);
    }
}
