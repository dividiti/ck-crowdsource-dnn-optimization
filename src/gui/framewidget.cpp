#include "framewidget.h"

#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>

#define FRAME_CONTENT_H 102
#define PREDICTIONS_COUNT 5

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

    auto layoutInfo = new QVBoxLayout;
    layoutInfo->setMargin(0);
    layoutInfo->setSpacing(4);
    for (int i = 0; i < PREDICTIONS_COUNT; i++)
    {
        auto label = new QLabel;
        label->setTextFormat(Qt::PlainText);
        layoutInfo->addWidget(label);
        _predictions << label;
    }

    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_imageView);
    layout->addSpacing(4);
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

void FrameWidget::showPredictions(const QVector<PredictionResult>& predictions)
{
    for (int i = 0; i < _predictions.size(); i++)
        if (i < predictions.size())
        {
            auto text = QString("%1 - %2")
                    .arg(predictions.at(i).probability, 0, 'f', 2)
                    .arg(predictions.at(i).description);
            setTrimmedText(_predictions.at(i), text);
        }
        else _predictions.at(i)->clear();
}

void FrameWidget::setTrimmedText(QLabel* label, const QString& text)
{
    static QFontMetrics metrics(label->font());
    static int targetWidth = label->width();
    label->setText(metrics.elidedText(text, Qt::ElideRight, targetWidth));
}
