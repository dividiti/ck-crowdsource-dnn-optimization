#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>
#include <QJsonObject>
#include <QList>

class CkEntry
{
public:
    QString uid;
    QString name;

    QString str() const;

    bool isEmpty() const { return uid.isEmpty(); }
};

//-----------------------------------------------------------------------------

class AppRunParams
{
public:
    QString dnnModelUid;
    bool startImmediately = false;

    bool isEmpty() const { return dnnModelUid.isEmpty(); }
};

//-----------------------------------------------------------------------------

class RecognitionScenario
{
public:
    void parseCK(const CkEntry& entry);

    QString str() const;
    QString html() const;

    QString title() const { return _title; }
    QString uid() const { return _uid; }

private:
    QString _title, _uid;
};

//-----------------------------------------------------------------------------

class RecognitionScenarios
{
public:
    void append(const CkEntry& entry);
    const QList<RecognitionScenario>& items() const { return _items; }

    bool isEmpty() const { return _items.isEmpty(); }
    QString str() const;

private:
    QList<RecognitionScenario> _items;
};

//-----------------------------------------------------------------------------

class ExperimentProbe
{
public:
    double time;
    double memory;
};

//-----------------------------------------------------------------------------

class ExperimentResult
{
public:
    int imagesCount;
    double totalTime;
    double timePerImage;
    double imagesPerSecond;
    double timePerBatch;
    double memoryPerImage;

    void reset();
    void accumulate(const ExperimentProbe& p);
};

//-----------------------------------------------------------------------------

#endif // APPMODELS_H
