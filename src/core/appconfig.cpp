#include "appconfig.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QDebug>

namespace ConfigKeys
{
    const QString remoteServer("remote_server_url");
    const QString email("email");
    const QString checkScenarioFilesMd5("check_scenario_files_md5");
    const QString imagesDir("images_dir");
    const QString imagesFilter("images_filter");
    const QString selectedScenario("selected_scenario_");
    const QString batchSize("batchSize");
} // namespace ConfigKeys

//-----------------------------------------------------------------------------

QSettings& AppConfig::config()
{
    static QSettings cfg(configFileName(), QSettings::IniFormat);
    return cfg;
}

QString AppConfig::sharedResourcesUrl()
{
    return QStringLiteral("http://cTuning.org/shared-computing-resources-json/ck.json");
}

QString AppConfig::sharedRepoUrl()
{
    return config().value(ConfigKeys::remoteServer).toString();
}

void AppConfig::setSharedRepoUrl(const QString& url)
{
    config().setValue(ConfigKeys::remoteServer, url);
}

QString AppConfig::email()
{
    return config().value(ConfigKeys::email).toString();
}

QString AppConfig::configFileName()
{
    return qApp->applicationDirPath() + QDir::separator() + "app.conf";
}

QString AppConfig::platformFeaturesCacheFile()
{
    return qApp->applicationDirPath() + QDir::separator() + "platformFeatures.json";
}

QString AppConfig::scenariosCacheFile()
{
    return qApp->applicationDirPath() + QDir::separator() + "recognitionScenarios.json";
}

QString AppConfig::scenariosDataDir()
{
    return qApp->applicationDirPath() + QDir::separator() + "openscience";
}

bool AppConfig::checkScenarioFilesMd5()
{
    return config().value(ConfigKeys::checkScenarioFilesMd5, true).toBool();
}

QString AppConfig::imagesDir()
{
    auto dir = config().value(ConfigKeys::imagesDir).toString();
    if (dir.isEmpty()) dir = "images";
    return QDir(qApp->applicationDirPath() + QDir::separator() + dir).absolutePath();
}

QStringList AppConfig::imagesFilter()
{
    auto filter = config().value(ConfigKeys::imagesFilter).toString();
    if (filter.isEmpty()) filter = "*.jpg;*.jpeg";
    return filter.split(";", QString::SkipEmptyParts);
}

int AppConfig::selectedScenarioIndex(int experimentIndex)
{
    return configValueInt(experimentKey(ConfigKeys::selectedScenario, experimentIndex), -1);
}

void AppConfig::setSelectedScenarioIndex(int experimentIndex, int scenarioIndex)
{
    config().setValue(experimentKey(ConfigKeys::selectedScenario, experimentIndex), scenarioIndex);
}

int AppConfig::batchSize(int experimentIndex)
{
    return configValueInt(experimentKey(ConfigKeys::batchSize, experimentIndex), 2);
}

void AppConfig::setBatchSize(int experimentIndex, int batchSize)
{
    config().setValue(experimentKey(ConfigKeys::batchSize, experimentIndex), batchSize);
}

int AppConfig::configValueInt(const QString& key, int defaultValue)
{
    bool ok;
    int value = config().value(key, defaultValue).toInt(&ok);
    return ok? value: defaultValue;
}

QString AppConfig::experimentKey(const QString& baseKey, int experimentIndex)
{
    return baseKey + QString::number(experimentIndex);
}
