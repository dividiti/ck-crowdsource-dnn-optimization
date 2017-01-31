#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>
#include <QList>
#include <QVector>

class CkEntry
{
public:
    QString uid;
    QString name;

    QString title() const { return name; }

    QString str() const;

    bool isEmpty() const { return uid.isEmpty(); }
};

//-----------------------------------------------------------------------------

class AppRunParams
{
public:
    QString engineUid;
    QString modelUid;
    bool startImmediately = false;
};

//-----------------------------------------------------------------------------

class PredictionResult
{
public:
    double probability = 0;
    QString description;
    QString id;
};

//-----------------------------------------------------------------------------

class ExperimentProbe
{
public:
    QString image;
    double time = 0;
    double memory = 0;
    QVector<PredictionResult> predictions;
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

#endif // APPMODELS_H
