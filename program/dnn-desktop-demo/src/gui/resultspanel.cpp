#include "imageview.h"
#include "experimentcontext.h"
#include "resultspanel.h"
#include "../ori/OriWidgets.h"
#include "appconfig.h"

#include <QBoxLayout>
#include <QLabel>
#include <QVariant>
#include <QDebug>
#include <QDateTime>
#include <QPushButton>

#define WORST_PREDICTED_IMAGE_W 160
#define WORST_PREDICTED_IMAGE_H 120

static QObject* spacing(int px) {
    return reinterpret_cast<QObject*>(px);
}

ResultsPanel::ResultsPanel(ExperimentContext *context, QWidget *parent)
    : QFrame(parent), _updateIntervalMs(AppConfig::fpsUpdateIntervalMs())
{
    setObjectName("resultsPanel");

    _context = context;
    connect(_context, &ExperimentContext::experimentStarted, this, &ResultsPanel::experimentStarted);
    connect(_context, &ExperimentContext::newImageResult, this, &ResultsPanel::newImageResult);
    connect(_context, &ExperimentContext::modeChanged, this, &ResultsPanel::updateOnModeChanged);
    connect(_context, &ExperimentContext::zoomChanged, this, &ResultsPanel::updateOnZoomChanged);

    _infoImagesPerSec = makeInfoLabel();
    _infoPrecision = makeInfoLabel();
    _infoMetricTop1 = makeInfoLabel();
    _infoMetricTop5 = makeInfoLabel();

    _worstPredictedImage = new ImageView(WORST_PREDICTED_IMAGE_W, WORST_PREDICTED_IMAGE_H);

    auto panelCounters = makePanel({ Ori::Gui::makeTitle("IMAGES PER SECOND"), _infoImagesPerSec });
    _panelPrecision = makePanel({ Ori::Gui::makeTitle("AVERAGE PRECISION"), _infoPrecision });
    auto panelMetricTop1 = makePanel({ Ori::Gui::makeTitle("TOP-1"), _infoMetricTop1 });
    auto panelMetricTop5 = makePanel({ Ori::Gui::makeTitle("TOP-5"), _infoMetricTop5 });
    _panelMetrics = new QFrame;
    _panelMetrics->setLayout(Ori::Gui::layoutH(0, 0, { panelMetricTop1, panelMetricTop5 }));
    _panelWorstPrediction = makePanel({
        Ori::Gui::makeTitle("WORST PREDICTION"),
        Ori::Gui::layoutH(0, 0, { 0, _worstPredictedImage, 0}),
    });

    auto buttonZoomIn = new QPushButton;
    buttonZoomIn->setObjectName("buttonZoomIn");
    buttonZoomIn->setToolTip(tr("Zoom in"));
    buttonZoomIn->setIcon(QIcon(":/tools/zoom-in"));
    connect(buttonZoomIn, SIGNAL(clicked(bool)), _context, SLOT(zoomIn()));

    auto buttonZoomOut = new QPushButton;
    buttonZoomOut->setObjectName("buttonZoomOut");
    buttonZoomOut->setToolTip(tr("Zoom out"));
    buttonZoomOut->setIcon(QIcon(":/tools/zoom-out"));
    connect(buttonZoomOut, SIGNAL(clicked(bool)), _context, SLOT(zoomOut()));

    auto buttonZoomActual = new QPushButton;
    buttonZoomActual->setObjectName("buttonZoomActual");
    buttonZoomActual->setToolTip(tr("Actual size"));
    buttonZoomActual->setIcon(QIcon(":/tools/zoom-to-actual-size"));
    connect(buttonZoomActual, SIGNAL(clicked(bool)), _context, SLOT(zoomActual()));

    _infoZoom = new QLabel;
    _infoZoom->setAlignment(Qt::AlignTop | Qt::AlignRight);
    _infoZoom->setProperty("qss-role", "link");

    auto zoomLayout = Ori::Gui::layoutH({buttonZoomIn, spacing(8), buttonZoomOut, 0, buttonZoomActual});
    _panelZoom = makePanel({
        Ori::Gui::layoutH({ Ori::Gui::makeTitle("ZOOM"), 0, _infoZoom }),
        zoomLayout
    });

    setLayout(Ori::Gui::layoutV(0, 0,
        { panelCounters, _panelPrecision, _panelZoom, _panelMetrics, _panelWorstPrediction, 0 }));

    resetInfo();
    updateOnModeChanged(AppConfig::currentMode().value<Mode>());
    updateOnZoomChanged(AppConfig::zoom());
}

QLabel* ResultsPanel::makeInfoLabel(const QString &role) {
    auto label = new QLabel;
    label->setProperty("qss-role", role.isEmpty() ? QString("info-label") : role);
    return label;
}

QFrame* ResultsPanel::makePanel(const std::initializer_list<QObject *> &items, const QString &objectName) {
    auto panel = new QFrame;
    panel->setProperty("qss-role", "results-panel");
    panel->setObjectName(objectName);
    panel->setLayout(Ori::Gui::layoutV(0, 0, items));
    return panel;
}

void ResultsPanel::experimentStarted(bool resume) {
    if (!resume) {
        resetInfo();
    }
}

void ResultsPanel::newImageResult(ImageResult ir) {
    qint64 curTimeMs = QDateTime::currentMSecsSinceEpoch();
    if (curTimeMs - _lastUpdateMs > _updateIntervalMs) {
        _infoImagesPerSec->setText(QString(QStringLiteral("%1")).arg(ir.imagesPerSecond(), 0, 'f', 2));
        _infoPrecision->setText(QString(QStringLiteral("%1")).arg(_context->precision().avg, 0, 'f', 2));
        _infoMetricTop1->setText(QString::number(_context->top1().avg, 'f', 2));
        _infoMetricTop5->setText(QString::number(_context->top5().avg, 'f', 2));

        double accuracyDelta = ir.accuracyDelta();
        if (accuracyDelta > _worstAccuracyDelta) {
            _worstAccuracyDelta = accuracyDelta;
            _worstPredictedImage->loadImage(ir.imageFile);
            _worstPredictedImage->setToolTip(QString(QStringLiteral("%1\nTop1: %2\nCorrect: %3"))
                                             .arg(ir.imageFile)
                                             .arg(ir.predictions[0].str())
                                             .arg(ir.findCorrect()->str()));
        }
        _lastUpdateMs = curTimeMs;
    }
}

void ResultsPanel::resetInfo() {
    _infoImagesPerSec->setText("N/A");
    _infoPrecision->setText("N/A");
    _infoMetricTop1->setText("N/A");
    _infoMetricTop5->setText("N/A");
    _worstAccuracyDelta = 0;
    _worstPredictedImage->clearImage();
    _worstPredictedImage->setToolTip("");
    _lastUpdateMs = 0;
}

void ResultsPanel::updateOnModeChanged(Mode mode) {
    bool v = mode.type == Mode::Type::CLASSIFICATION;
    _panelMetrics->setVisible(v);
    _panelWorstPrediction->setVisible(v);
    _panelPrecision->setVisible(!v);
    _panelZoom->setVisible(!v);
}

void ResultsPanel::updateOnZoomChanged(double z) {
    int p = z * 100;
    _infoZoom->setText(QString("<span style='color:#969C9E'>%1%</span>").arg(p));
}
