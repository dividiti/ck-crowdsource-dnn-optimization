#include "imageview.h"
#include "experimentcontext.h"
#include "resultspanel.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QLabel>
#include <QVariant>

#define WORST_PREDICTED_IMAGE_W 180
#define WORST_PREDICTED_IMAGE_H 135

ResultsPanel::ResultsPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
    setObjectName("resultsPanel");

    _context = context;
    connect(_context, &ExperimentContext::experimentStarted, this, &ResultsPanel::experimentStarted);
    connect(_context, &ExperimentContext::experimentResultReady, this, &ResultsPanel::experimentResultReady);

    _infoTimePerImage = new QLabel;
    _infoTimePerImage->setProperty("qss-role", "info-label");

    _infoImagesPerSec = new QLabel;
    _infoImagesPerSec->setProperty("qss-role", "info-label-small");

    auto panelCounters = new QFrame;
    panelCounters->setProperty("qss-role", "results-panel");
    panelCounters->setLayout(Ori::Gui::layoutV(0, 0, {
        Utils::makeTitle("TIME PER\nIMAGE (FPS)"), _infoTimePerImage, _infoImagesPerSec }));

    _worstPredictedImage = new ImageView(WORST_PREDICTED_IMAGE_W, WORST_PREDICTED_IMAGE_H);

    auto panelWorstPrediction = new QFrame;
    panelWorstPrediction->setObjectName("worstPredictionPanel");
    panelWorstPrediction->setProperty("qss-role", "results-panel");
    panelWorstPrediction->setLayout(Ori::Gui::layoutV(0, 0, {
        Utils::makeTitle("WORST PREDICTION"),
        Ori::Gui::layoutH(0, 0, { 0, _worstPredictedImage, 0}),
    }));

    setLayout(Ori::Gui::layoutV(0, 0,
        { panelCounters, panelWorstPrediction, 0 }));
}

void ResultsPanel::experimentStarted()
{
    _infoTimePerImage->setText("N/A");
    _infoImagesPerSec->clear();
}

void ResultsPanel::experimentResultReady()
{
    auto r = _context->experimentResult();
    _infoTimePerImage->setText(QString::number(r.timePerImage, 'f', 2));
    _infoImagesPerSec->setText(QString(QStringLiteral("(%1)")).arg(r.imagesPerSecond, 0, 'f', 2));
    if (r.worstPredictionFlag)
    {
        _worstPredictedImage->loadImage(r.worstPredictedImage);
        _worstPredictedImage->setToolTip(QString(QStringLiteral("%1\nTop1: %2\nCorrect: %3"))
                                         .arg(r.worstPredictedImage)
                                         .arg(r.worstPredictionTop1.str())
                                         .arg(r.worstPredictionCorrect.str()));
    }
}
