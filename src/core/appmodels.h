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
    enum RunMode { Normal, EditStyle };

    QString engineUid;
    QString modelUid;
    QString imagesUid;
    bool startImmediately = false;
    RunMode runMode = Normal;
};

//-----------------------------------------------------------------------------

class PredictionResult
{
public:
    double accuracy = 0;
    QString labels;
    QString index;
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
    void accumulate(const ExperimentProbe *p);
};

//-----------------------------------------------------------------------------

class ImagesDataset
{
public:
    QString title() const { return _title; }
    QString imagesPath() const { return _imagesPath; }

    QString str() const;
    bool isEmpty() const { return _imagesPath.isEmpty(); }

    bool hasCorrectnessMap() const { return _hasCorrectnessMap; }
    void buildCorrectnessMap();

private:
    QString _title, _imagesPath;

    QString _valFile, _wordsFile;
    bool _hasCorrectnessMap = false;

    friend class CK;
};

//-----------------------------------------------------------------------------

class DnnEngine
{
public:
    QString title() const { return _title; }
    QString library() const { return _library; }

    QString str() const;
    bool isEmpty() const { return _library.isEmpty(); }

private:
    QString _title, _library;

    friend class CK;
};

#endif // APPMODELS_H
