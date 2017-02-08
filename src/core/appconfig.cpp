#include "appconfig.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QDebug>

QSettings& AppConfig::config()
{
    static QSettings cfg(configFileName(), QSettings::IniFormat);
    return cfg;
}

QString AppConfig::configFileName()
{
    return qApp->applicationDirPath() + QDir::separator() + "app.conf";
}

QString AppConfig::styleSheetFileName()
{
    return qApp->applicationDirPath() + QDir::separator() + "app.qss";
}

int AppConfig::selectedEngineIndex(int experimentIndex)
{
    return configValueInt(experimentKey("selected_engine", experimentIndex), -1);
}

void AppConfig::setSelectedEngineIndex(int experimentIndex, int engineIndex)
{
    config().setValue(experimentKey("selected_engine", experimentIndex), engineIndex);
}

int AppConfig::selectedModelIndex(int experimentIndex)
{
    return configValueInt(experimentKey("selected_model", experimentIndex), -1);
}

void AppConfig::setSelectedModelIndex(int experimentIndex, int modelIndex)
{
    config().setValue(experimentKey("selected_model", experimentIndex), modelIndex);
}

int AppConfig::selectedImagesIndex(int experimentIndex)
{
    return configValueInt(experimentKey("selected_images", experimentIndex), -1);
}

void AppConfig::setSelectedImagesIndex(int experimentIndex, int imagesIndex)
{
    config().setValue(experimentKey("selected_images", experimentIndex), imagesIndex);
}

int AppConfig::batchSize(int experimentIndex)
{
    return configValueInt(experimentKey("batch_size", experimentIndex), 2);
}

void AppConfig::setBatchSize(int experimentIndex, int batchSize)
{
    config().setValue(experimentKey("batch_size", experimentIndex), batchSize);
}

QString AppConfig::ckReposPath()
{
    return config().value("ck_repos_path").toString();
}

QString AppConfig::ckBinPath()
{
    return config().value("ck_bin_path").toString();
}

QString AppConfig::ckExeName()
{
    return config().value("ck_exe_name").toString();
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

QString AppConfig::localSubdir(const QString& key, const QString& defaultDir)
{
    auto defaultPath = qApp->applicationDirPath() + QDir::separator() + defaultDir;
    auto path = configValueStr(key, defaultPath);
    QDir d(path);
    if (!d.exists())
        d.mkpath(path);
    return d.absolutePath();
}
