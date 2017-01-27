#include "appconfig.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QDebug>

namespace ConfigKeys
{
    const QString remoteServer("remote_server_url");
    //const QString email("email");
    //const QString checkScenarioFilesMd5("check_scenario_files_md5");
    //const QString selectedScenario("selected_scenario_");
    //const QString batchSize("batchSize");
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

/*QString AppConfig::email()
{
    return config().value(ConfigKeys::email).toString();
}*/

QString AppConfig::configFileName()
{
    return qApp->applicationDirPath() + QDir::separator() + "app.conf";
}

QString AppConfig::platformFeaturesCacheFile()
{
    return qApp->applicationDirPath() + QDir::separator() + "platformFeatures.json";
}

/*QString AppConfig::scenariosCacheFile()
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
}*/

QString AppConfig::imagesDir()
{
    return QDir(qApp->applicationDirPath() + QDir::separator() +
                configValueStr("images_dir", "images")).absolutePath();
}

QStringList AppConfig::imagesFilter()
{
    return configValueStr("images_filter", "*.jpg;*.jpeg").split(";", QString::SkipEmptyParts);
}

int AppConfig::selectedScenarioIndex(int experimentIndex)
{
    return configValueInt(experimentKey("selected_scenario", experimentIndex), -1);
}

void AppConfig::setSelectedScenarioIndex(int experimentIndex, int scenarioIndex)
{
    config().setValue(experimentKey("selected_scenario", experimentIndex), scenarioIndex);
}

int AppConfig::batchSize(int experimentIndex)
{
    return configValueInt(experimentKey("batch_size", experimentIndex), 2);
}

void AppConfig::setBatchSize(int experimentIndex, int batchSize)
{
    config().setValue(experimentKey("batch_size", experimentIndex), batchSize);
}

QString AppConfig::ckPath()
{
    return configValueStr("ck_path", "~/CK"); // TODO: default value is platform specific
}

QString AppConfig::ckBinPath()
{
    auto path = config().value("ck_bin_path").toString();
    if (path.isEmpty())
        qCritical() << "CK bin path not found in config";
        // TODO: ask user where CK is installed
    return path;
}

QString AppConfig::ckExeName()
{
    return configValueStr("ck_exe_name", "./ck"); // TODO: default value is platform specific
}

int AppConfig::configValueInt(const QString& key, int defaultValue)
{
    bool ok;
    int value = config().value(key, defaultValue).toInt(&ok);
    return ok? value: defaultValue;
}

QString AppConfig::configValueStr(const QString& key, const QString& defaultValue)
{
    auto value = config().value(key, defaultValue).toString();
    return value.isEmpty()? defaultValue: value;
}

QString AppConfig::experimentKey(const QString& baseKey, int experimentIndex)
{
    return baseKey + "_" + QString::number(experimentIndex);
}

QStringList AppConfig::ckArgs()
{
    return config().value("ck_args").toString().split(' ', QString::SkipEmptyParts);
}
