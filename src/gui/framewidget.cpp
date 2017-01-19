#include "framewidget.h"

#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPainter>

QPixmap getSampleImage(const QPixmap& frame)
{
    int index = qrand()%10 + 1;
    auto sample = QPixmap(QString(":/sample/%1").arg(index));

    int h = 128;
    int w = h * sample.width() / sample.height();

    QPixmap image(frame.width(), frame.height());
    image.fill(Qt::transparent); // force transparency
    QPainter painter(&image);
    painter.drawPixmap((frame.width()-w)/2, (frame.height()-h)/2, w, h, sample);
    painter.drawPixmap(0, 0, frame);
    return image;
}

FrameWidget::FrameWidget(QWidget *parent) : QWidget(parent)
{
    auto image = getSampleImage(QPixmap(":/images/frame"));

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(image.width());

    auto imageView = new QLabel;
    imageView->setPixmap(image);

    auto layoutInfo = new QVBoxLayout;
    layoutInfo->setMargin(6);
    layoutInfo->setSpacing(4);
    layoutInfo->addWidget(new QLabel("variant 1"));
    layoutInfo->addWidget(new QLabel("variant 2"));
    layoutInfo->addWidget(new QLabel("variant 3"));

    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(imageView);
    layout->addLayout(layoutInfo);
    layout->addStretch();

    setLayout(layout);
}
