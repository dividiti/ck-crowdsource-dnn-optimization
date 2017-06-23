#ifndef EXPERIMENTCONTEXT_H
#define EXPERIMENTCONTEXT_H

#include "appmodels.h"

#include <QObject>
#include <QList>

class ExperimentContext : public QObject
{
    Q_OBJECT

public:

    struct Stat {
        double avg = 0;
        double min = 0;
        double max = 0;
        qint64 count = 0;

        void clear() {
            avg = 0;
            min = 0;
            max = 0;
            count = 0;
        }

        void add(double v) {
            ++count;
            avg = (avg*(count - 1) + v)/count;
            min = 0 == min || min > v ? v : min;
            max = 0 == max || max < v ? v : max;
        }
    };

    ExperimentContext();

    void startExperiment(bool resume);
    void stopExperiment();
    void notifyModeChanged(const Mode& mode);
    bool isExperimentStarted() const { return _isExperimentStarted; }

    Stat duration() const { return _duration; }
    Stat precision() const { return _precision; }
    Stat top1() const { return _top1; }
    Stat top5() const { return _top5; }

    Mode::Type mode() const { return _mode; }
    int batchSize() const { return _batchSize; }

    bool resumable() const { return _isExperimentInterrupted && Mode::Type::RECOGNITION == mode() && !_lastResult.originalImageFile.isEmpty(); }
    ImageResult lastResult() const { return _lastResult; }

    bool hasAggregatedResults() const { return _duration.count > 0; }

signals:
    void experimentStarted(bool);
    void experimentStopping();
    void experimentFinished();
    void newImageResult(ImageResult);
    void modeChanged(Mode);
    void zoomChanged(double, bool);
    void effectiveZoomChanged(double);

public slots:
    void zoomIn();
    void zoomOut();
    void zoomActual();
    void zoomToFit();

private slots:
    void aggregateResults(ImageResult);

private:
    bool _isExperimentStarted = false;
    bool _isExperimentInterrupted = false;
    Stat _duration;
    Stat _precision;
    Stat _top1;
    Stat _top5;
    Mode::Type _mode;
    int _batchSize;
    ImageResult _lastResult;

    void clearAggregatedResults();
    void emitZoomChanged(double, bool);
};

#endif // EXPERIMENTCONTEXT_H
