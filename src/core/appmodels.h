#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>

class SharedResourcesInfo
{
public:
    QString url;
    QString note;
    int weight;
};


class PlatformFeatures
{
public:
};


class RecognitionScenario
{
public:
    const QString& title() const { return _title; }

private:
    QString _title;
};


#endif // APPMODELS_H
