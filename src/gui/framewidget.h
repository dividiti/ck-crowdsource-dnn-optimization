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
    QList<QLabel*> _predictions;

    const QPixmap& frame() const;
    QPixmap getSampleImage();
    QPixmap getFramedImage(const QPixmap& source);

    void setTrimmedText(QLabel* label, const QString& text);
};

#endif // FRAMEWIDGET_H
