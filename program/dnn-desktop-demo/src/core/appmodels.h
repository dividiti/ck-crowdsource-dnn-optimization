#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>
#include <QList>
#include <QVector>
#include <QMetaType>

class PredictionResult
{
public:
    double accuracy = 0;
    QString labels;
    int index;
    bool isCorrect;

    QString str() const {
        return QString(QStringLiteral("%1 - %2 %3"))
            .arg(accuracy).arg(labels)
            .arg(isCorrect ? QString(QStringLiteral(" CORRECT")) : QString());
    }
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

struct Program {
    QString uoa;
    QString name;
    QString outputFile;

    QString title() const {
        return name;
    }

    bool operator==(const Program& o) const {
        return uoa == o.uoa;
    }
};
Q_DECLARE_METATYPE(Program)

//-----------------------------------------------------------------------------

struct Model {
    QString uoa;
    QString name;

    QString title() const {
        return name;
    }

    bool operator==(const Model& o) const {
        return uoa == o.uoa;
    }
};
Q_DECLARE_METATYPE(Model)

//-----------------------------------------------------------------------------

struct Dataset {
    QString auxUoa;
    QString auxName;
    QString valUoa;
    QString valName;

    QString title() const {
        return auxName;
    }

    bool operator==(const Dataset& o) const {
        return auxUoa == o.auxUoa;
    }
};
Q_DECLARE_METATYPE(Dataset)

#endif // APPMODELS_H
