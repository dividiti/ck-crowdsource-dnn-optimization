#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

class SharedRepoInfo
{
public:
    QString url() const { return _url; }
    QString note() const { return _note; }
    int weight() const { return _weight; }

    QString parseJson(const QByteArray& text);

private:
    QString _url;
    QString _note;
    int _weight;
};


class PlatformFeatures
{
public:
    QJsonObject json;

    QString parseJson(const QByteArray& text);
};


class RecognitionScenario
{
};


class RecognitionScenarios
{
public:
    QList<QJsonObject> jsons;

    QString parseJson(const QByteArray& text);
};


#endif // APPMODELS_H
