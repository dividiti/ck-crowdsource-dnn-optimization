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

    void load(const ImageResult& imageResult);

private:
    ImageView *_imageView;
    QList<PredictionView*> _predictions;
};

#endif // FRAMEWIDGET_H
