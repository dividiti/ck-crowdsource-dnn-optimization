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

    bool isEmpty() const { return _url.isEmpty(); }
    QString str() const;

    void parseJson(const QByteArray& text);

    void loadFromConfig();
    void saveToConfig();

private:
    QString _url;
    QString _note;
    int _weight = 0;
};

//-----------------------------------------------------------------------------

class PlatformFeatures
{
public:
    QJsonObject json;

    bool isEmpty() const { return json.isEmpty(); }
    QString str() const;

    void parseJson(const QByteArray& text);

    void loadFromFile(const QString& path);
    void saveToFile(const QString& path);
};

//-----------------------------------------------------------------------------

class RecognitionScenario
{
};

//-----------------------------------------------------------------------------

class RecognitionScenarios
{
public:
    QList<QJsonObject> jsons;

    bool isEmpty() const { return jsons.isEmpty(); }
    QString str() const;

    void parseJson(const QByteArray& text);

    void loadFromFile(const QString& path);
    void saveToFile(const QString& path);
};


#endif // APPMODELS_H
