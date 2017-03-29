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
    QMapIterator<QString, int> iter(ir.recognizedObjects);
    QString text = "";
    while (iter.hasNext()) {
        iter.next();
        int expected = ir.expectedObjects.contains(iter.key()) ? ir.expectedObjects[iter.key()] : 0;
        text += QString(QStringLiteral("<b>%1:</b> %2 found, %3 expected<br>")).arg(iter.key()).arg(iter.value()).arg(expected);
    }
    descriptionLabel->setText(text);
}

void RecognitionWidget::clear() {
    imageLabel->clear();
    descriptionLabel->clear();
}
