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

    // NB: take copy not reference! because of _fileDownloads rearranges its
    // content when another QNetworkReply is finished and its entry is removed
    FileDownloadWork download = _fileDownloads[reply];

    if (!QDir().mkpath(download.file.fullPath()))
        return processDownloadProgress(reply, download, tr("Unable to create target directory"));

    if (reply->error())
        return processDownloadProgress(reply, download, reply->errorString());

    QFile file(download.file.fullFileName());
    if (!file.open(QIODevice::WriteOnly))
        return processDownloadProgress(reply, download, tr("Unable to create target file: %1").arg(file.errorString()));

    if (file.write(reply->readAll()) == -1)
        return processDownloadProgress(reply, download, tr("Fail writing file: %1").arg(file.errorString()));

    file.close();

    if (AppConfig::checkScenarioFilesMd5() && !download.file.checkMD5())
        return processDownloadProgress(reply, download, tr("MD5 verification failed"));

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
            AppEvents::error(tr("Download failed:\n\n%1").arg(status->error())); // TODO: show scenarion title here
        }
        emit filesDownloadComplete(download.scenarioIndex);
    }

    _fileDownloads.remove(reply);
}

void ScenariosProvider::deleteScenarioFiles(int scenarioIndex)
{
    if (scenarioIndex < 0 || scenarioIndex >= _current.items().size())
        return AppEvents::error(tr("Unable to delete files for scenario: invalid scenario index %1").arg(scenarioIndex));

    const RecognitionScenario& scenario = _current.items().at(scenarioIndex);

    QStringList report;
    // remove files
    for (const RecognitionScenarioFileItem& file: scenario.files())
    {
        qDebug() << "Deleting file" << file.fullFileName();
        QFile f(file.fullFileName());
        if (!f.remove())
        {
            qDebug() << "Unable to delete" << f.errorString();
            report << tr("Unable to delete file %1: %2").arg(f.fileName(), f.errorString());
        }
    }
    // try remove dirs if they are empty
    QDir dataRoot(AppConfig::scenariosDataDir());
    for (const RecognitionScenarioFileItem& file: scenario.files())
    {
        qDebug() << "Deleting dir" << file.fullPath();
        dataRoot.rmpath(file.path());
    }

    if (!report.isEmpty())
        AppEvents::error(tr("There are errors while deleting scenario files:\n\n%1").arg(report.join("\n")));

    auto status = _scenarioDownloads[scenarioIndex];
    if (status) prepareDownloadStatus(scenario, status);
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
        auto status = new ScenarioDownloadStatus;
        prepareDownloadStatus(_current.items().at(i), status);
        _scenarioDownloads.insert(i, status);
    }
}

void ScenariosProvider::prepareDownloadStatus(const RecognitionScenario& scenario, ScenarioDownloadStatus* status)
{
    qDebug() << "Preparing scenario" << scenario.title();
    status->_totalFiles = scenario.files().size();
    if (scenario.allFilesAreLoaded())
    {
        qDebug() << "All files loaded";
        status->_status = ScenarioDownloadStatus::AllLoaded;
        status->_loadedFiles = scenario.files().size();
    }
    else
    {
        qDebug() << "Files not loaded";
        status->_status = ScenarioDownloadStatus::NoFiles;
        status->_loadedFiles = 0;
    }
}
