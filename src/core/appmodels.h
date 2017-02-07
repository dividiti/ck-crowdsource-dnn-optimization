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
    int index;
    bool isCorrect;

    QString str() const;
};

//-----------------------------------------------------------------------------

class ExperimentProbe
{
public:
    QString image;
    double time;
    double memory;
    QVector<PredictionResult> predictions;
    PredictionResult correctInfo;
    bool isTop1;
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

    bool worstPredictionFlag;
    double worstPredictionMarker;
    PredictionResult worstPredictionCorrect;
    PredictionResult worstPredictionTop1;
    QString worstPredictedImage;

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
    QString valFile() const { return _valFile; }

    QString str() const { return _title + ": " + _imagesPath; }
    bool isEmpty() const { return _imagesPath.isEmpty(); }

private:
    QString _title, _imagesPath;
    QString _valFile, _meanFile, _labelsFile;

    friend class CK;
};

//-----------------------------------------------------------------------------

class ImageEntry
{
public:
    QString fileName;
    int correctIndex;
};

class ImagesBank
{
public:
    ImagesBank(const QString& imagesDir, const QString& valFile);

    const QList<ImageEntry>& images() const { return _images; }
    bool isEmpty() const { return _images.isEmpty(); }
private:
    QList<ImageEntry> _images;
};

#endif // APPMODELS_H
