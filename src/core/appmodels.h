#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>
#include <QJsonObject>
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
    void parseJson(const QByteArray& text);
    const QJsonObject& json() const { return _json; }

    bool isEmpty() const { return _json.isEmpty(); }
    QString str() const;

    void loadFromFile(const QString& path);
    void saveToFile(const QString& path) const;

    QString osName() const { return _osName; }

private:
    QJsonObject _json;
    QString _osName;
};

//-----------------------------------------------------------------------------

class RecognitionScenario
{
public:
    bool parseJson(const QJsonObject& json);
    const QJsonObject& json() const { return _json; }

    bool isEmpty() const { return _json.isEmpty(); }
    QString str() const;

    long fileSizeBytes() const { return _fileSizeBytes; }
    QString fileSizeMB() const { return _fileSizeMB; }
    QString title() const { return _title; }

private:
    long _fileSizeBytes = 0;
    QString _fileSizeMB, _title;
    QJsonObject _json;
};

//-----------------------------------------------------------------------------

class RecognitionScenarios
{
public:
    bool isEmpty() const { return _items.isEmpty(); }
    QString str() const;

    void parseJson(const QByteArray& text);

    void loadFromFile(const QString& path);
    void saveToFile(const QString& path) const;

private:
    QList<RecognitionScenario> _items;
};


#endif // APPMODELS_H
