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

QString FileDownloadWork::errorTitle() const
{
    return qApp->tr("Error while downloading scenario file '%1' from %2 into %3: ").arg(file.name(), file.url(), file.fullPath());
}

//-----------------------------------------------------------------------------

ScenariosProvider::ScenariosProvider(RemoteDataAccess *network, QObject *parent) : QObject(parent), _network(network)
{
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

void ScenariosProvider::downloadScenarioFiles(int scenarioIndex)
{
    if (scenarioIndex < 0 || scenarioIndex >= _current.items().size())
        return AppEvents::error(tr("Unable to download files for scenario: invalid scenario index %1").arg(scenarioIndex));

    int fileIndex = 0;
    for (const RecognitionScenarioFileItem& file: _current.items().at(scenarioIndex).files())
    {
        AppEvents::info(QString("Downloading file %1 from %2 into %3").arg(file.name(), file.url(), file.fullPath()));

        auto targetDir = file.fullPath();
        QDir dir;
        if (!dir.mkpath(targetDir))
        {
            AppEvents::error(tr("Unable to create target directory %1").arg(targetDir));
            emit scenarioFileDownloaded(scenarioIndex, fileIndex, false);
            continue;
        }
        qDebug() << "Target directory created or already exists:" << targetDir;

        auto reply = _network->get(file.url()); // start downloading async
        connect(reply, &QNetworkReply::finished, this, &ScenariosProvider::queryDownloadScenarioFile_finished);
        _fileDownloads.insert(reply, { scenarioIndex, fileIndex, file });
        fileIndex++;
    }
}

void ScenariosProvider::queryDownloadScenarioFile_finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !_fileDownloads.contains(reply)) return;

    const FileDownloadWork& fileDownload = _fileDownloads[reply];


    if (reply->error())
    {
        AppEvents::error(fileDownload.errorTitle() + reply->errorString());
        emit scenarioFileDownloaded(fileDownload.scenarioIndex, fileDownload.fileIndex, false);
        _fileDownloads.remove(reply);
        return;
    }

    QFile file(fileDownload.file.fullFileName());
    if (!file.open(QIODevice::WriteOnly))
    {
        AppEvents::error(fileDownload.errorTitle() + tr("unable to create file: %2").arg(fileDownload.errorTitle(), file.errorString()));
        emit scenarioFileDownloaded(fileDownload.scenarioIndex, fileDownload.fileIndex, false);
        _fileDownloads.remove(reply);
        return;
    }

    if (file.write(reply->readAll()) == -1)
    {
        AppEvents::error(fileDownload.errorTitle() + tr("fail writing file: %2").arg(file.errorString()));
        emit scenarioFileDownloaded(fileDownload.scenarioIndex, fileDownload.fileIndex, false);
        _fileDownloads.remove(reply);
        return;
    }

    emit scenarioFileDownloaded(fileDownload.scenarioIndex, fileDownload.fileIndex, true);
    _fileDownloads.remove(reply);

    qDebug() << fileDownload.file.name() << "OK";
}
