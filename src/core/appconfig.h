#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class AppConfig
{
public:
    static QString sharedResourcesUrl();

    static QString sharedRepoUrl();
    static void setSharedRepoUrl(const QString& url);

    static QString email();

    static QString platformFeaturesCacheFile();
    static QString scenariosCacheFile();

private:
    static QString configFileName();
    static QSettings& config();
};

#endif // APPCONFIG_H
