#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>

#include "appmodels.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QPixmap;
QT_END_NAMESPACE

class FrameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FrameWidget(QWidget *parent = 0);

    void loadImage(const QString& path);
    void showPredictions(const QVector<PredictionResult> &predictions);

private:
    QLabel *_imageView;
    QList<class PredictionView*> _predictions;

    QPixmap fitImage(const QPixmap& source);
};

#endif // FRAMEWIDGET_H
