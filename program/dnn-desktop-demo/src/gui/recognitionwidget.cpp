#include "recognitionwidget.h"
#include "appmodels.h"
#include "appconfig.h"

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>
#include <QMap>
#include <QFrame>

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

RecognitionWidget::RecognitionWidget(QWidget *parent) : QWidget(parent) {
    imageLabel = new QLabel;

    auto hl = new QHBoxLayout;
    hl->setMargin(0);
    hl->addStretch();
    hl->addWidget(imageLabel);
    hl->addStretch();

    auto frame = new QFrame;
    frame->setLayout(hl);

    QVBoxLayout* l = new QVBoxLayout;
    l->setMargin(0);
    l->addWidget(frame);

    descriptionLabel = new QLabel;
    descriptionLabel->setProperty("qss-role", "recognition-label");
    descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l->addWidget(descriptionLabel);

    setLayout(l);
}

void RecognitionWidget::load(const ImageResult& ir) {
    QPixmap pixmap(ir.imageFile);
    int h = AppConfig::recognitionImageHeight();
    if (0 < h) {
        pixmap = pixmap.scaledToHeight(h);
    }
    imageLabel->setPixmap(pixmap);
    imageLabel->setToolTip(ir.imageFile);
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
}

void RecognitionWidget::clear() {
    imageLabel->clear();
    descriptionLabel->clear();
}
