#include "imageview.h"
#include "experimentcontext.h"
#include "resultspanel.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QLabel>
#include <QVariant>
#include <QDebug>

#define WORST_PREDICTED_IMAGE_W 160
#define WORST_PREDICTED_IMAGE_H 120

ResultsPanel::ResultsPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent) {
    setObjectName("resultsPanel");

    _context = context;
    connect(_context, &ExperimentContext::experimentStarted, this, &ResultsPanel::experimentStarted);
    connect(_context, &ExperimentContext::newImageResult, this, &ResultsPanel::newImageResult);

    _infoImagesPerSec = makeInfoLabel();
    _infoMetricTop1 = makeInfoLabel();
    _infoMetricTop5 = makeInfoLabel();

    _worstPredictedImage = new ImageView(WORST_PREDICTED_IMAGE_W, WORST_PREDICTED_IMAGE_H);

    auto panelCounters = makePanel({ Ori::Gui::makeTitle("IMAGES PER SECOND"), _infoImagesPerSec });
    auto panelMetricTop1 = makePanel({ Ori::Gui::makeTitle("TOP-1"), _infoMetricTop1 });
    auto panelMetricTop5 = makePanel({ Ori::Gui::makeTitle("TOP-5"), _infoMetricTop5 });
    auto panelMetrics = Ori::Gui::layoutH(0, 0, { panelMetricTop1, panelMetricTop5 });
    auto panelWorstPrediction = makePanel({
        Ori::Gui::makeTitle("WORST PREDICTION"),
        Ori::Gui::layoutH(0, 0, { 0, _worstPredictedImage, 0}),
    });

    setLayout(Ori::Gui::layoutV(0, 0,
        { panelCounters, panelMetrics, panelWorstPrediction, 0 }));

    resetInfo();
}

QLabel* ResultsPanel::makeInfoLabel(const QString &role) {
    auto label = new QLabel;
    label->setProperty("qss-role", role.isEmpty()? QString("info-label"): role);
    return label;
}

QFrame* ResultsPanel::makePanel(const std::initializer_list<QObject *> &items, const QString &objectName) {
    auto panel = new QFrame;
    panel->setProperty("qss-role", "results-panel");
    panel->setObjectName(objectName);
    panel->setLayout(Ori::Gui::layoutV(0, 0, items));
    return panel;
}

void ResultsPanel::experimentStarted() {
    resetInfo();
}

void ResultsPanel::newImageResult(ImageResult ir) {
    ++_imageCount;
    if (ir.correctAsTop1()) {
        ++_top1Count;
    }
    if (ir.correctAsTop5()) {
        ++_top5Count;
    }
    _infoImagesPerSec->setText(QString(QStringLiteral("%1")).arg(ir.imagesPerSecond(), 0, 'f', 2));
    _infoMetricTop1->setText(QString::number((double)_top1Count / _imageCount, 'f', 2));
    _infoMetricTop5->setText(QString::number((double)_top5Count / _imageCount, 'f', 2));

    double accuracyDelta = ir.accuracyDelta();
    if (accuracyDelta > _worstAccuracyDelta) {
        _worstAccuracyDelta = accuracyDelta;
        _worstPredictedImage->loadImage(ir.imageFile);
        _worstPredictedImage->setToolTip(QString(QStringLiteral("%1\nTop1: %2\nCorrect: %3"))
                                         .arg(ir.imageFile)
                                         .arg(ir.predictions[0].str())
                                         .arg(ir.findCorrect()->str()));
    }
}

void ResultsPanel::resetInfo() {
    _infoImagesPerSec->setText("N/A");
    _infoMetricTop1->setText("N/A");
    _infoMetricTop5->setText("N/A");
    _top1Count = 0;
    _top5Count = 0;
    _imageCount = 0;
    _worstAccuracyDelta = 0;
}
