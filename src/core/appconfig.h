#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

class AppConfig
{
public:
    static QString sharedResourcesUrl();
    static QString remoteServerUrl();

private:
    static QString configFileName();
};

#endif // APPCONFIG_H
