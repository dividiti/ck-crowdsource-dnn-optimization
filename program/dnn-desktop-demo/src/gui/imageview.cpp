#include "imageview.h"

#include <QPainter>
#include <QPaintEvent>

ImageView::ImageView(int w, int h, QWidget *parent) : QFrame(parent) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(w, h);
}

void ImageView::loadImage(const QString& file) {
    _image.load(file);
    repaint();
}

void ImageView::clearImage() {
    _image = QImage();
    repaint();
}

void ImageView::paintEvent(QPaintEvent *event) {
    double aspect = _image.width() / double(_image.height());
    int targetH = event->rect().height();
    int targetW = targetH * aspect;
    int fullW = event->rect().width();
    QPainter p(this);
    p.drawImage(QRect((fullW - targetW)/2, 0, targetW, targetH), _image, _image.rect());
}
