#ifndef RESULTSPANEL_H
#define RESULTSPANEL_H

#include "appmodels.h"
#include <QFrame>

QT_BEGIN_NAMESPACE
class QLabel;
class QLayout;
QT_END_NAMESPACE

class ExperimentContext;
class ImageView;
class InfoLabel;

class ResultsPanel : public QFrame
{
    Q_OBJECT
public:
    explicit ResultsPanel(ExperimentContext* context, QWidget *parent = 0);

private slots:
    void experimentStarted();
    void newImageResult(ImageResult);
    void updateOnModeChanged(Mode);

private:
    ExperimentContext* _context;
    QLabel* _infoImagesPerSec;
    QLabel* _infoPrecision;
    QLabel* _infoMetricTop1;
    QLabel* _infoMetricTop5;
    ImageView *_worstPredictedImage;
    QFrame* _panelMetrics;
    QFrame* _panelWorstPrediction;
    QFrame* _panelPrecision;

    double _worstAccuracyDelta;

    const qint64 _updateIntervalMs;
    qint64 _lastUpdateMs;

    QLabel* makeInfoLabel(const QString& role = QString());
    QFrame* makePanel(const std::initializer_list<QObject*>& items, const QString& objectName = QString());
    void resetInfo();
};

#endif // RESULTSPANEL_H
