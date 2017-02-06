#include "framewidget.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

#define FRAME_CONTENT_W 220
#define FRAME_CONTENT_H 165
#define PREDICTIONS_COUNT 5
#define PROB_LABEL_W 45
#define PROB_LABEL_H 18
#define PROB_CORRECT_COLOR "#009688"

//-----------------------------------------------------------------------------

class ImageView : public QFrame
{
public:
    void loadImage(const QString& file)
    {
        _image.load(file);
        repaint();
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

class PredictionProbLabel : public QLabel
{
public:
    PredictionProbLabel()
    {
        _correctBrush = QBrush(QColor(PROB_CORRECT_COLOR));
        _correctPen = QPen(Qt::NoPen);
        _correctText = QPen(Qt::white);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setFixedSize(PROB_LABEL_W, PROB_LABEL_H);
    }
    void setValue(double value, bool correct)
    {
        _text = QString::number(value, 'f', 2);
        _isCorrect = correct;
        update();
    }
protected:
    void paintEvent(QPaintEvent *event) override
    {
        auto r = event->rect();
        QPainter p(this);
        if (_isCorrect)
        {
            p.setPen(_correctPen);
            p.setBrush(_correctBrush);
            p.setRenderHint(QPainter::Antialiasing, true);
            p.drawRoundedRect(r, r.height()/2, r.height()/2);
            p.setPen(_correctText);
        }
        p.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, _text);
    }
private:
    bool _isCorrect = false;
    QString _text;
    QBrush _correctBrush;
    QPen _correctPen, _correctText;
};

//-----------------------------------------------------------------------------

class PredictionView : public QFrame
{
public:
    PredictionView(int index)
    {
        _prob = new PredictionProbLabel;
        _prob->setTextFormat(Qt::PlainText);
        _prob->setObjectName("predictionProb");
        _prob->setProperty("qss-prediction-line", index);

        _descr = new QLabel;
        _descr->setTextFormat(Qt::PlainText);
        _descr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        _descr->setObjectName("predictionDescr");
        _descr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        _descr->setProperty("qss-prediction-line", index);

        // It seems that label's width does not take into account nor margin or padding values
        // that are set via style-sheet, and therefore elided description string is calculated
        // with incorrect length (see setDescr); so we have to set distance betweeen
        // _prob and _descr via code as layout spacing, but not via style-sheet.
        setLayout(Ori::Gui::layoutH(0, 16, {
            Ori::Gui::spacing(4), _prob, _descr, Ori::Gui::spacing(4),
        }));
    }
    void setProb(double value, bool correct)
    {
        _prob->setValue(value, correct);
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
    PredictionProbLabel *_prob;
    QLabel *_descr;
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

    setLayout(Ori::Gui::layoutV(0, 8, { _imageView, layoutInfo }));
}

void FrameWidget::loadImage(const QString& path, int imageCorrectIndex)
{
    _imageView->loadImage(path);
    _imageView->setToolTip(QString(QStringLiteral("%1 (%2)")).arg(path).arg(imageCorrectIndex));
}

void FrameWidget::showPredictions(const QVector<PredictionResult>& predictions, int correctIndex)
{
    for (int i = 0; i < _predictions.size(); i++)
        if (i < predictions.size())
        {
            PredictionView* pv = _predictions.at(i);
            const PredictionResult& p = predictions.at(i);
            pv->setProb(p.accuracy, p.index == correctIndex);
            pv->setDescr(p.labels);
            pv->setToolTip(QString(QStringLiteral("%1 - %2 (%3) %4"))
                .arg(p.accuracy).arg(p.labels).arg(p.index)
                .arg(p.index == correctIndex? "CORRECT": ""));
        }
        else _predictions.at(i)->clear();
}
