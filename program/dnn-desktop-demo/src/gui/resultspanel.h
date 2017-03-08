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
    void experimentResultReady();
    void newImageResult(ImageResult);

private:
    ExperimentContext* _context;
    QLabel *_infoImagesPerSec;
    QLabel *_infoMetricTop1, *_infoMetricTop5;
    ImageView *_worstPredictedImage;

    int _top1Count;
    int _top5Count;
    int _imageCount;
    double _worstAccuracyDelta;

    QLabel* makeInfoLabel(const QString& role = QString());
    QFrame* makePanel(const std::initializer_list<QObject*>& items, const QString& objectName = QString());
    void resetInfo();
};

#endif // RESULTSPANEL_H
