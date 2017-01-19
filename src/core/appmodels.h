#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>

QT_BEGIN_NAMESPACE
//class QJsonObject;
QT_END_NAMESPACE

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
};


class RecognitionScenario
{
public:
    const QString& title() const { return _title; }

private:
    QString _title;
};


#endif // APPMODELS_H
