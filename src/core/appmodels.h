#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>
#include <QList>
#include <QVector>

class AppRunParams
{
public:
    enum RunMode { Normal, EditStyle };

    QString modelUid;
    QString engineUid;
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

class DnnModel
{
public:
    QString title() const { return _title; }
    QString modelFile() const { return _modelFile; }
    QString weightsFile() const { return _weightsFile; }

    QString str() const { return _title + ": " + _weightsFile; }
    bool isEmpty() const { return _modelFile.isEmpty() || _weightsFile.isEmpty(); }

private:
    QString _title, _modelFile, _weightsFile;

    friend class CK;
};

//-----------------------------------------------------------------------------

class DnnEngine
{
public:
    QString title() const { return _title; }
    QString library() const { return _library; }
    QStringList paths() const { return _paths; }

    QString str() const { return _title + ": " + _library; }
    bool isEmpty() const { return _library.isEmpty(); }

private:
    QString _title, _library;
    QStringList _paths;

    friend class CK;
};

//-----------------------------------------------------------------------------

class ImagesDataset
{
public:
    QString title() const { return _title; }
    QString imagesPath() const { return _imagesPath; }
    QString meanFile() const { return _meanFile; }
    QString labelsFile() const { return _labelsFile; }

    QString str() const { return _title + ": " + _imagesPath; }
    bool isEmpty() const { return _imagesPath.isEmpty(); }

    bool hasCorrectnessMap() const { return _hasCorrectnessMap; }
    void buildCorrectnessMap();

private:
    QString _title, _imagesPath;
    QString _valFile, _meanFile, _labelsFile;
    bool _hasCorrectnessMap = false;

    friend class CK;
};

#endif // APPMODELS_H
