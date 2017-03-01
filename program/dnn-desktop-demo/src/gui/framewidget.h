#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QFrame>

#include "appmodels.h"

class ImageView;
class PredictionView;

class FrameWidget : public QFrame
{
    Q_OBJECT

public:
    explicit FrameWidget(QWidget *parent = 0);

    void loadImage(const QString &imageFile, PredictionResult& correctInfo);
    void showPredictions(const QVector<PredictionResult> &predictions);

private:
    ImageView *_imageView;
    QList<PredictionView*> _predictions;
};

#endif // FRAMEWIDGET_H
