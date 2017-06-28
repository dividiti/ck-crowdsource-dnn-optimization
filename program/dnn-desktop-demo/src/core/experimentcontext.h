#ifndef EXPERIMENTCONTEXT_H
#define EXPERIMENTCONTEXT_H

#include "appmodels.h"

#include <QObject>
#include <QVector>
#include <QProcess>

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

    bool resumable() const {
        return _isExperimentInterrupted && Mode::Type::RECOGNITION == mode() && !lastResult().originalImageFile.isEmpty();
    }

    ImageResult lastResult() const {
        return _results.empty() ? ImageResult() : _results.last();
    }

    int currentResult() const {
        return _current_result;
    }

    int resultCount() const {
        return _results.size();
    }

    bool hasAggregatedResults() const { return _duration.count > 0; }

    void clearAggregatedResults();

signals:
    void experimentStarted(bool);
    void experimentStopping();
    void experimentFinished();
    void newImageResult(ImageResult);
    void currentResultChanged(int, int, ImageResult);
    void modeChanged(Mode);
    void zoomChanged(double, bool);
    void effectiveZoomChanged(double);
    void publishStarted();
    void publishFinished(bool);

public slots:
    void zoomIn();
    void zoomOut();
    void zoomActual();
    void zoomToFit();

    void gotoNextResult();
    void gotoPrevResult();
    void gotoFirstResult();
    void gotoLastResult();

    void publishResults();

private slots:
    void aggregateResults(ImageResult);
    void publishResultsFinished(int, QProcess::ExitStatus);
    void publishResultsError(QProcess::ProcessError);

private:
    bool _isExperimentStarted = false;
    bool _isExperimentInterrupted = false;
    Stat _duration;
    Stat _precision;
    Stat _top1;
    Stat _top5;
    Mode::Type _mode;
    int _batchSize;
    QVector<ImageResult> _results;
    int _current_result;

    Program _program;
    Model _model;
    Dataset _dataset;

    QProcess* _publisher;

    void emitZoomChanged(double, bool);
    void emitCurrentResult();
};

#endif // EXPERIMENTCONTEXT_H
