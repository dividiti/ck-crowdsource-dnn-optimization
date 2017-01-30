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
    const QList<RecognitionScenario>& items() const { return _items; }

    bool isEmpty() const { return _items.isEmpty(); }
    QString str() const;

    void loadFromCK(const QList<CkEntry>& entries);

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
