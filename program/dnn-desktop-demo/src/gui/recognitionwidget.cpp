#include "recognitionwidget.h"
#include "appmodels.h"
#include "appconfig.h"
#include "experimentcontext.h"

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>
#include <QMap>
#include <QFrame>
#include <QScrollArea>
#include <QScrollBar>
#include <QColor>
#include <QPainter>
#include <QDebug>

static const QMap<QString, QString> ICONS {
    {"car", ":/images/ico-auto"},
    {"cyclist", ":/images/ico-bike"},
    {"pedestrian", ":/images/ico-pedestrian"},
    {"person", ":/images/ico-pedestrian"},

    {"aeroplane", ":/images/ico-plane"},
    {"bicycle", ":/images/ico-bicycle"},
    {"bird", ":/images/ico-bird"},
    {"boat", ":/images/ico-boat"},
    {"bottle", ":/images/ico-bottle"},
    {"bus", ":/images/ico-bus"},
    {"cat", ":/images/ico-cat"},
    {"chair", ":/images/ico-chair"},
    {"cow", ":/images/ico-cow"},
    {"diningtable", ":/images/ico-diningtable"},
    {"dog", ":/images/ico-dog"},
    {"horse", ":/images/ico-horse"},
    {"motorbike", ":/images/ico-motorbike"},
    {"pottedplant", ":/images/ico-pottedplant"},
    {"sheep", ":/images/ico-sheep"},
    {"sofa", ":/images/ico-sofa"},
    {"train", ":/images/ico-train"},
    {"tvmonitor", ":/images/ico-tvmonitor"}
};

RecognitionWidget::RecognitionWidget(ExperimentContext* ctx, QWidget *parent) : QWidget(parent), context(ctx) {
    connect(context, &ExperimentContext::zoomChanged, this, &RecognitionWidget::rescale);

    scroll = new QScrollArea;
    scroll->setStyleSheet("background-color:transparent; border: none;");
    scroll->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
    scroll->verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");

    QVBoxLayout* l = new QVBoxLayout;
    l->setMargin(0);
    l->addWidget(scroll);

    descriptionLabel = new QLabel;
    descriptionLabel->setProperty("qss-role", "recognition-label");
    descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    l->addWidget(descriptionLabel);

    setLayout(l);
}

RecognitionWidget::~RecognitionWidget() {
    disconnect(context, &ExperimentContext::zoomChanged, this, &RecognitionWidget::rescale);
}

static QPixmap makeLarger(const QPixmap& pixmap, int width, int height) {
    if (width <= pixmap.width() && height <= pixmap.height()) {
        return pixmap;
    }
    QPixmap ret(qMax(width, pixmap.width()), qMax(height, pixmap.height()));
    ret.fill(QColor::fromRgbF(0, 0, 0, 0));
    QPainter p(&ret);
    int x = qMax(0, (width - pixmap.width())/2);
    int y = qMax(0, (height - pixmap.height())/2);
    p.drawPixmap(x, y, pixmap, 0, 0, 0, 0);
    return ret;
}

static QPixmap scale(const QPixmap& pixmap, double factor) {
    if (1 == factor || 0 >= factor) {
        return pixmap;
    }
    return pixmap.scaled(pixmap.width() * factor, pixmap.height() * factor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QPixmap RecognitionWidget::polishPixmap(const QPixmap& pmap) {
    QPixmap pixmap = pmap;
    int h = AppConfig::recognitionImageHeight();
    if (0 < h) {
        pixmap = pixmap.scaledToHeight(h);
    } else {
        pixmap = scale(pixmap, AppConfig::zoom());
    }
    if (scroll->width() > pixmap.width() || scroll->height() > pixmap.height()) {
        pixmap = makeLarger(pixmap, scroll->width() + 50, scroll->height() + 50);
    }
    return pixmap;
}

void RecognitionWidget::load(const ImageResult& ir) {
    QString text = "<style>"
                   "    th { "
                   "        text-align: center;"
                   "        background-color: #313c42;"
                   "        color: #64ffda;"
                   "        font-weight: bold;"
                   "        font-size: 13px;"
                   "        padding-top: 14px;"
                   "        padding-bottom: 14px;"
                   "    }"
                   "    td { "
                   "        text-align: center;"
                   "        background-color: #2b373d;"
                   "        color: #ffffff;"
                   "        font-size: 15px;"
                   "        padding-top: 8px;"
                   "        padding-bottom: 8px;"
                   "    }"
                   "    td.obj, th.obj { "
                   "        background-color: #313c42;"
                   "    }"
                   "</style>"
            "<table width=\"100%\"><tr><th class=\"obj\">OBJECT</th><th>FOUND</th><th>EXPECTED</th><th>FALSE POSITIVES</th><th>PRECISION</th><th>RECALL</th></tr>";
    QMapIterator<QString, int> iter(ir.recognizedObjects);
    while (iter.hasNext()) {
        iter.next();
        QString obj = iter.key();
        auto spec = ir.labelSpec(obj);
        if (ICONS.contains(obj)) {
            obj = "<img src=\"" + ICONS[obj] + "\">";
        }
        text += QString("<tr><td class=\"obj\">%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                .arg(obj).arg(spec.identified).arg(spec.expected).arg(spec.falsePositive).arg(spec.precision(), 0, 'g', 2).arg(spec.recall(), 0, 'g', 2);
    }
    text += "</table>";
    descriptionLabel->setText(text);

    origPixmap = QPixmap(ir.imageFile);
    imageTooltip = ir.imageFile;
    updateScrollArea();
}

void RecognitionWidget::updateScrollArea() {
    if (origPixmap.isNull()) {
        return;
    }
    auto imageLabel = new QLabel;
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel->setPixmap(polishPixmap(origPixmap));
    imageLabel->setToolTip(imageTooltip);
    auto old = scroll->widget();
    if (old) {
        old->deleteLater();
    }
    scroll->setWidget(imageLabel);
    scroll->verticalScrollBar()->setValue(scroll->verticalScrollBar()->maximum() / 2);
    scroll->horizontalScrollBar()->setValue(scroll->horizontalScrollBar()->maximum() / 2);
}

void RecognitionWidget::rescale(double) {
    updateScrollArea();
}
