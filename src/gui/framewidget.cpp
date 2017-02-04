#include "framewidget.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

#define FRAME_CONTENT_W 160
#define FRAME_CONTENT_H 147
#define PREDICTIONS_COUNT 5

class ImageView : public QFrame
{
public:
    void loadImage(const QString& file)
    {
        _image.load(file);
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        double aspect = _image.width() / double(_image.height());
        int targetH = event->rect().height();
        int targetW = targetH * aspect;
        int fullW = event->rect().width();
        QPainter p(this);
        p.drawImage(QRect((fullW - targetW)/2, 0, targetW, targetH), _image, _image.rect());
    }

private:
    QImage _image;
};

//-----------------------------------------------------------------------------

class PredictionView : public QWidget
{
public:
    PredictionView(int index)
    {
        _prob = new QLabel;
        _prob->setTextFormat(Qt::PlainText);
        _prob->setObjectName("predictionProb");
        _prob->setProperty("qss-prediction-line", index);
        _descr = new QLabel;
        _descr->setTextFormat(Qt::PlainText);
        _descr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        _descr->setObjectName("predictionDescr");
        _descr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        _descr->setProperty("qss-prediction-line", index);
        setLayout(Ori::Gui::layoutH(0, 0, {_prob, Ori::Gui::spacing(16), _descr}));
    }
    void setProb(double value)
    {
        _prob->setText(QString::number(value, 'f', 2));
    }
    void setDescr(const QString& text)
    {
        static QFontMetrics metrics(_descr->font());
        _descr->setText(metrics.elidedText(text, Qt::ElideRight, _descr->width()));
    }
    void clear()
    {
        _prob->clear();
        _descr->clear();
    }
private:
    QLabel *_prob, *_descr;
};

//-----------------------------------------------------------------------------

FrameWidget::FrameWidget(QWidget *parent) : QFrame(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(FRAME_CONTENT_W);

    _imageView = new ImageView;
    _imageView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _imageView->setFixedSize(FRAME_CONTENT_W, FRAME_CONTENT_H);
    _imageView->setObjectName("frameImage");

    auto layoutInfo = new QVBoxLayout;
    layoutInfo->setMargin(0);
    layoutInfo->setSpacing(4);
    for (int i = 0; i < PREDICTIONS_COUNT; i++)
    {
        auto view = new PredictionView(i+1);
        layoutInfo->addWidget(view);
        _predictions << view;
    }

    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(4);
    layout->addWidget(_imageView);
    layout->addLayout(layoutInfo);
    layout->addStretch();

    setLayout(layout);
}

void FrameWidget::loadImage(const QString& path)
{
    _imageView->loadImage(path);
}

void FrameWidget::showPredictions(const QVector<PredictionResult>& predictions)
{
    for (int i = 0; i < _predictions.size(); i++)
        if (i < predictions.size())
        {
            _predictions.at(i)->setProb(predictions.at(i).accuracy);
            _predictions.at(i)->setDescr(predictions.at(i).labels);
        }
        else _predictions.at(i)->clear();
}
