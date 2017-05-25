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

static const QMap<QString, QString> ICONS{ {"car", ":/images/ico-auto"}, {"cyclist", ":/images/ico-bike"}, {"pedestrian", ":/images/ico-pedestrian"} };

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
        int expected = ir.expectedObjects.contains(iter.key()) ? ir.expectedObjects[iter.key()] : 0;
        int identified = iter.value();
        int falsePositive = ir.falsePositiveObjects.contains(iter.key()) ? ir.falsePositiveObjects[iter.key()] : 0;
        int trueObjects = identified - falsePositive;
        double precision = 0 == identified ? (0 == expected ? 1 : 0) : (double)trueObjects / (double)identified;
        double recall = 0 == expected ? (0 == identified ? 1 : 0) : (double)trueObjects / (double)expected;
        QString obj = iter.key();
        if (ICONS.contains(obj)) {
            obj = "<img src=\"" + ICONS[obj] + "\">";
        }
        text += QString("<tr><td class=\"obj\">%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                .arg(obj).arg(identified).arg(expected).arg(falsePositive).arg(precision, 0, 'g', 2).arg(recall, 0, 'g', 2);
    }
    text += "</table>";
    descriptionLabel->setText(text);
}

void RecognitionWidget::clear() {
    imageLabel->clear();
    descriptionLabel->clear();
}
