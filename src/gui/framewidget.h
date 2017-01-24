#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>

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
    void showInfo(const QString& info);

private:
    QLabel *_imageView, *_textInfo;

    const QPixmap& frame() const;
    QPixmap getSampleImage();
    QPixmap getFramedImage(const QPixmap& source);
};

#endif // FRAMEWIDGET_H
