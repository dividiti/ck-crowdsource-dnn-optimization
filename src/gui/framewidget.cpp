#include "framewidget.h"

#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPainter>

#define FRAME_CONTENT_H 102

QPixmap FrameWidget::getSampleImage()
{
    int index = qrand()%10 + 1;
    auto sample = QPixmap(QString(":/sample/%1").arg(index));
    return getFramedImage(sample);
}

FrameWidget::FrameWidget(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(frame().width());

    _imageView = new QLabel;
    _textInfo = new QLabel;

    auto layoutInfo = new QVBoxLayout;
    layoutInfo->setMargin(6);
    layoutInfo->setSpacing(4);
    layoutInfo->addWidget(_textInfo);

    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_imageView);
    layout->addLayout(layoutInfo);
    layout->addStretch();

    setLayout(layout);
}

void FrameWidget::loadImage(const QString& path)
{
    _imageView->setPixmap(getFramedImage(QPixmap(path)));
}

const QPixmap& FrameWidget::frame() const
{
    static QPixmap frame(":/images/frame");
    return frame;
}

QPixmap FrameWidget::getFramedImage(const QPixmap& source)
{
    int h = FRAME_CONTENT_H;
    int w = h * source.width() / source.height();

    auto s = frame().size();
    QPixmap image(s);
    image.fill(Qt::transparent); // force transparency
    QPainter painter(&image);
    painter.drawPixmap((s.width()-w)/2, (s.height()-h)/2, w, h, source);
    painter.drawPixmap(0, 0, frame());
    return image;
}

void FrameWidget::showInfo(const QString& info)
{
    _textInfo->setText(info);
}
