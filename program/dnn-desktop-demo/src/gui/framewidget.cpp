#include "framewidget.h"
#include "imageview.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QMapIterator>

#define FRAME_CONTENT_W 220
#define FRAME_CONTENT_H 165
#define PREDICTIONS_COUNT 5
#define PROB_LABEL_W 45
#define PROB_LABEL_H 18
#define PROB_CORRECT_COLOR "#009688"

class PredictionProbLabel : public QLabel
{
public:
    PredictionProbLabel() {
        _correctBrush = QBrush(QColor(PROB_CORRECT_COLOR));
        _correctPen = QPen(Qt::NoPen);
        _correctText = QPen(Qt::white);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setFixedSize(PROB_LABEL_W, PROB_LABEL_H);
    }

    void setValue(double value, bool correct) {
        _text = QString::number(value, 'f', 2);
        _isCorrect = correct;
        update();
    }

protected:

    void paintEvent(QPaintEvent *event) override {
        auto r = event->rect();
        QPainter p(this);
        if (_isCorrect) {
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
    PredictionView(int index) {
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

    void setProb(double value, bool correct) {
        _prob->setValue(value, correct);
    }

    void setDescr(const QString& text) {
        static QFontMetrics metrics(_descr->font());
        _descr->setText(metrics.elidedText(text, Qt::ElideRight, _descr->width()));
    }

    void clear() {
        _prob->clear();
        _descr->clear();
    }

private:
    PredictionProbLabel *_prob;
    QLabel *_descr;
};

//-----------------------------------------------------------------------------

FrameWidget::FrameWidget(QWidget *parent) : QFrame(parent) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(FRAME_CONTENT_W);

    _imageView = new ImageView(FRAME_CONTENT_W, FRAME_CONTENT_H);
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

    setLayout(Ori::Gui::layoutV(0, 8, { _imageView, layoutInfo, 0 }));
}

void FrameWidget::load(const ImageResult& ir) {
    _imageView->loadImage(ir.imageFile);
    if (!ir.isEmpty()) {
        _imageView->setToolTip(QString(QStringLiteral("%1\n%2")).arg(ir.imageFile).arg(ir.correctLabels));
        for (int i = 0; i < _predictions.size(); i++) {
            if (i < ir.predictions.size()) {
                PredictionView* pv = _predictions.at(i);
                const PredictionResult& p = ir.predictions.at(i);
                pv->setProb(p.accuracy, p.isCorrect);
                pv->setDescr(p.labels);
                pv->setToolTip(p.str());
            } else {
                _predictions.at(i)->clear();
            }
        }

    } else if (!ir.recognizedObjects.isEmpty()) {
        _imageView->setToolTip("");
        for (auto p : _predictions) {
            p->clear();
        }
        int i = 0;
        QMapIterator<QString, int> iter(ir.recognizedObjects);
        while (iter.hasNext() && i < _predictions.size()) {
            ++i;
            iter.next();
            int expected = ir.expectedObjects.contains(iter.key()) ? ir.expectedObjects[iter.key()] : 0;
            PredictionView* pv = _predictions.at(i);
            pv->setDescr(QString(QStringLiteral("%1: %2 out of %3")).arg(iter.key()).arg(iter.value()).arg(expected));
            pv->setToolTip("");
        }
    }
}
