#include "recognitionwidget.h"
#include "appmodels.h"

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QVariant>

static const int FRAME_WIDTH = 1240;
static const int FRAME_HEIGHT = 375;

RecognitionWidget::RecognitionWidget(QWidget *parent) : QWidget(parent) {
    imageLabel = new QLabel;
    imageLabel->setScaledContents(true);
    QVBoxLayout* l = new QVBoxLayout;
    l->setMargin(0);
    l->addWidget(imageLabel);

    descriptionLabel = new QLabel;
    descriptionLabel->setProperty("qss-role", "recognition-label");
    l->addWidget(descriptionLabel);
    setLayout(l);
}

void RecognitionWidget::load(const ImageResult& ir) {
    imageLabel->setPixmap(QPixmap(ir.imageFile));
    imageLabel->setToolTip(ir.imageFile);
    QString text = "<style>td, th { padding-left: 1em; text-align: left; }</style>"
            "<table><tr><th>Object</th><th>Found</th><th>Expected</th><th>False positives</th><th>Precision</th><th>Recall</th></tr>";
    QMapIterator<QString, int> iter(ir.recognizedObjects);
    while (iter.hasNext()) {
        iter.next();
        int expected = ir.expectedObjects.contains(iter.key()) ? ir.expectedObjects[iter.key()] : 0;
        int identified = iter.value();
        int falsePositive = ir.falsePositiveObjects.contains(iter.key()) ? ir.falsePositiveObjects[iter.key()] : 0;
        int trueObjects = identified - falsePositive;
        double precision = 0 == identified ? (0 == expected ? 1 : 0) : (double)trueObjects / (double)identified;
        double recall = 0 == expected ? (0 == identified ? 1 : 0) : (double)trueObjects / (double)expected;
        text += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                .arg(iter.key()).arg(identified).arg(expected).arg(falsePositive).arg(precision).arg(recall);
    }
    text += "</table>";
    descriptionLabel->setText(text);
}

void RecognitionWidget::clear() {
    imageLabel->clear();
    descriptionLabel->clear();
}
