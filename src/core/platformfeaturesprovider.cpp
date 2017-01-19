#include "appconfig.h"
#include "appevents.h"
#include "platformfeaturesprovider.h"

#include <QDebug>
#include <QFile>

void PlatformFeaturesProvider::queryPlatformFeatures()
{
    PlatformFeatures features;

    QString res = loadCachedPlatformFeatures(features);
    if (!res.isEmpty())
        return AppEvents::error(res);

    if (features.json.isEmpty())
        return collectPlatformFeatures();

    emit platformFeaturesReceived(features);

}

QString PlatformFeaturesProvider::loadCachedPlatformFeatures(PlatformFeatures& features)
{
    auto path = AppConfig::platformFeaturesCacheFile();

    AppEvents::info(tr("Loading cached platform features from %1...").arg(path));

    QFile file(path);
    if (!file.exists())
    {
        AppEvents::info(tr("File not found"));
        return QString();
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return tr("Unable to open file: %1").arg(file.errorString());

    auto res = features.parseJson(file.readAll());
    if (!res.isEmpty())
        return res;

    return QString();
}

void PlatformFeaturesProvider::collectPlatformFeatures()
{
    AppEvents::info(tr("Collecting platform features..."));

    PlatformFeatures features;
    // TODO: do smth clever
    emit platformFeaturesReceived(features);
}
