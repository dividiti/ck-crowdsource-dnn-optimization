#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>
#include <QList>
#include <QVector>
#include <QMetaType>

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

Q_DECLARE_METATYPE(PredictionResult)

//-----------------------------------------------------------------------------

class ImageResult
{
public:
    QString imageFile;
    double duration;
    QVector<PredictionResult> predictions;
    QString correctLabels;

    bool correctAsTop1() const {
        return !predictions.isEmpty() && predictions[0].labels == correctLabels;
    }

    const PredictionResult* findCorrect() const {
        for (int i = 0; i < predictions.size(); ++i) {
            if (predictions[i].labels == correctLabels) {
                return &predictions[i];
            }
        }
        return nullptr;
    }

    bool correctAsTop5() const {
        return nullptr != findCorrect();
    }

    bool isEmpty() const {
        return predictions.isEmpty();
    }

    double imagesPerSecond() {
        return 1.0 / duration;
    }

    double accuracyDelta() {
        const PredictionResult* c = findCorrect();
        return nullptr != c ? predictions[0].accuracy - c->accuracy : 0;
    }
};

Q_DECLARE_METATYPE(ImageResult)

//-----------------------------------------------------------------------------

class ExperimentProbe
{
public:
    QString image;
    double time;
    double memory;
    QVector<PredictionResult> predictions;
    PredictionResult correctInfo;
    bool correctAsTop1;
    bool correctAsTop5;
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
    QStringList deps() const { return _deps; }

    QString str() const { return _title + ": " + _library; }
    bool isEmpty() const { return _library.isEmpty(); }

private:
    QString _title, _library;
    QStringList _deps;

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
