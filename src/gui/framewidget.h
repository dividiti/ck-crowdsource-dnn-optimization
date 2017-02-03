#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>

#include "appmodels.h"

class ImageView;
class PredictionView;

class FrameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FrameWidget(QWidget *parent = 0);

    void loadImage(const QString& path);
    void showPredictions(const QVector<PredictionResult> &predictions);

private:
    ImageView *_imageView;
    QList<PredictionView*> _predictions;
};

#endif // FRAMEWIDGET_H
