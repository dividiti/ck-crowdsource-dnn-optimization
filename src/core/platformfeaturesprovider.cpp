#include "appconfig.h"
#include "appevents.h"
#include "platformfeaturesprovider.h"

void PlatformFeaturesProvider::queryPlatformFeatures(const QString& sharedRepoUrl)
{
    AppEvents::info(tr("Collecting platform features %1").arg(sharedRepoUrl));

    PlatformFeatures features;

    // TODO: do smth more clever
    features.loadFromFile(AppConfig::platformFeaturesCacheFile());

    emit platformFeaturesReceived(features);
}

PlatformFeatures PlatformFeaturesProvider::loadFromCache()
{
    PlatformFeatures features;
    features.loadFromFile(AppConfig::platformFeaturesCacheFile());

    if (!features.isEmpty())
        _current = features;

    return features;
}

void PlatformFeaturesProvider::setCurrent(const PlatformFeatures& features)
{
    _current = features;

    features.saveToFile(AppConfig::platformFeaturesCacheFile());
}
