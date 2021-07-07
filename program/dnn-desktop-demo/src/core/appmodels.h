#ifndef APPMODELS_H
#define APPMODELS_H

#include <QString>
#include <QList>
#include <QVector>
#include <QMap>
#include <QMetaType>

#include <utility>

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

struct LabelSpec {
    int identified;
    int expected;
    int falsePositive;

    int trueObjects() const { return identified - falsePositive; }

    double precision() const { return 0 == identified ? (0 == expected ? 1 : 0) : (double)trueObjects() / (double)identified; }

    double recall() const { return 0 == expected ? (0 == identified ? 1 : 0) : (double)trueObjects() / (double)expected; }
};

struct ImageObject {
    QString label;
    float score = 1;
    float xmin = 0;
    float ymin = 0;
    float xmax = 0;
    float ymax = 0;
    bool ground_truth = false;

    float width() const {
        return xmax - xmin;
    }

    float height() const {
        return ymax - ymin;
    }
};

enum Difficulty {
  UNASSIGNED = -2,
  UNKNOWN = -1,
  EASY = 0,
  MODERATE = 1,
  HARD = 2
};

class ImageResult
{
public:
    QString imageFile;
    QString originalImageFile;
    double duration;
    QVector<PredictionResult> predictions;
    QString correctLabels;

    QMap<QString, int> recognizedObjects;
    QMap<QString, int> expectedObjects;
    QMap<QString, int> falsePositiveObjects;
    QVector<ImageObject> detections;
    QVector<ImageObject> groundTruth;

    QMap<QString, QVector<double>> rollingAP;
    double rollingMeanAP;

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
        return predictions.isEmpty() && recognizedObjects.isEmpty() && imageFile.isEmpty();
    }

    double imagesPerSecond() const {
        return 1.0 / duration;
    }

    double accuracyDelta() const {
        const PredictionResult* c = findCorrect();
        return nullptr != c ? predictions[0].accuracy - c->accuracy : 0;
    }

    LabelSpec labelSpec(const QString& label) const {
        const QString& key = label;
        LabelSpec ret;
        ret.identified = recognizedObjects.contains(key) ? recognizedObjects[key] : 0;
        ret.expected = expectedObjects.contains(key) ? expectedObjects[key] : 0;
        ret.falsePositive = falsePositiveObjects.contains(key) ? falsePositiveObjects[key] : 0;
        return ret;
    }

    double precision() const {
        if (recognizedObjects.isEmpty()) {
            return 0;
        }
        QMapIterator<QString, int> iter(recognizedObjects);
        double ret = 0;
        while (iter.hasNext()) {
            iter.next();
            ret += labelSpec(iter.key()).precision();
        }
        return ret / recognizedObjects.size();
    }
};

Q_DECLARE_METATYPE(ImageResult)

//-----------------------------------------------------------------------------

struct Mode {
    enum Type { CLASSIFICATION, RECOGNITION };

    const Type type;

    QString title() const {
        switch (type) {
        case Type::CLASSIFICATION:
            return "Object classification";
        case Type::RECOGNITION:
            return "Object detection";
        default:
            return "Unknown";
        }
    }

    QString name() const {
        return title().toLower().replace(" ", "-");
    }

    Mode(Type t = CLASSIFICATION) : type(t) {}

    bool operator==(const Mode& o) const {
        return type == o.type;
    }
};
Q_DECLARE_METATYPE(Mode)

//-----------------------------------------------------------------------------

struct Engine {
    enum Type { UNKNOWN, CAFFE, TF };

    static Type parse(QString s) {
        s = s.toLower();
        if (s == "caffe") {
            return CAFFE;
        } else if (s == "tf") {
            return TF;
        } else {
            return UNKNOWN;
        }
    }

    static QString toString(Type t) {
        if (t == CAFFE) {
            return "caffe";
        } else if (t == TF) {
            return "tf";
        } else {
            return "unknown";
        }
    }
};

//-----------------------------------------------------------------------------

struct Program {
    QString programUoa;
    QString targetUoa;
    QString name;
    QString outputFile;
    QString exe;
    QString targetDir;
    QString targetVersion;
    QString targetDepsInfo;
    bool supportsWebcam;

    Engine::Type engine;

    QString title() const {
        return name;
    }

    bool operator==(const Program& o) const {
        return targetUoa == o.targetUoa;
    }

    bool operator<(const Program& o) const {
        return title() < o.title();
    }
};
Q_DECLARE_METATYPE(Program)

//-----------------------------------------------------------------------------

struct Model {
    QString uoa;
    QString name;

    Engine::Type engine;

    QString title() const {
        return name;
    }

    bool operator==(const Model& o) const {
        return uoa == o.uoa;
    }

    bool operator<(const Model& o) const {
        return title() < o.title();
    }
};
Q_DECLARE_METATYPE(Model)

//-----------------------------------------------------------------------------

struct Dataset {
    QString auxUoa;
    QString auxName;
    QString valUoa;
    QString valName;
    QString cmdKey = "use_continuous";
    QMap<QString, QString> env;

    QString title() const {
        return valName;
    }

    bool operator==(const Dataset& o) const {
        return valUoa == o.valUoa;
    }

    bool operator<(const Dataset& o) const {
        return title() < o.title();
    }
};
Q_DECLARE_METATYPE(Dataset)

#endif // APPMODELS_H
