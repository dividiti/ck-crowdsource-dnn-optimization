#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

class AppConfig
{
public:
    static QString sharedResourcesUrl();
    static QString sharedRepoUrl();
    static void setSharedRepoUrl(const QString& url);

private:
    static QString configFileName();
};

#endif // APPCONFIG_H
