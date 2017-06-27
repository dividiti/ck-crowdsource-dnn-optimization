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

ResultsPanel::ResultsPanel(ExperimentContext *context, QWidget *parent)
    : QFrame(parent), _updateIntervalMs(AppConfig::fpsUpdateIntervalMs())
{
    setObjectName("resultsPanel");

    _context = context;
    connect(_context, &ExperimentContext::experimentStarted, this, &ResultsPanel::experimentStarted);
    connect(_context, &ExperimentContext::newImageResult, this, &ResultsPanel::newImageResult);
    connect(_context, &ExperimentContext::currentResultChanged, this, &ResultsPanel::currentResultChanged);
    connect(_context, &ExperimentContext::modeChanged, this, &ResultsPanel::updateOnModeChanged);
    connect(_context, &ExperimentContext::zoomChanged, this, &ResultsPanel::updateOnEffectiveZoomChanged);
    connect(_context, &ExperimentContext::effectiveZoomChanged, this, &ResultsPanel::updateOnEffectiveZoomChanged);

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

    auto buttonZoomToFit = new QPushButton;
    buttonZoomToFit->setObjectName("buttonZoomToFit");
    buttonZoomToFit->setToolTip(tr("Zoom to fit"));
    buttonZoomToFit->setIcon(QIcon(":/tools/zoom-to-fit"));
    connect(buttonZoomToFit, SIGNAL(clicked(bool)), _context, SLOT(zoomToFit()));

    _infoZoom = new QLabel;
    _infoZoom->setAlignment(Qt::AlignTop | Qt::AlignRight);
    _infoZoom->setProperty("qss-role", "link");

    auto zoomLayout = Ori::Gui::layoutH({buttonZoomActual, 0, buttonZoomOut, 0, buttonZoomIn, 0, buttonZoomToFit});
    _panelZoom = makePanel({
        Ori::Gui::layoutH({ Ori::Gui::makeTitle("ZOOM"), 0, _infoZoom }),
        zoomLayout
    });

    auto buttonFirst = new QPushButton;
    buttonFirst->setObjectName("buttonFirst");
    buttonFirst->setToolTip(tr("First image"));
    buttonFirst->setIcon(QIcon(":/tools/first-img"));
    connect(buttonFirst, SIGNAL(clicked(bool)), _context, SLOT(gotoFirstResult()));

    auto buttonPrev = new QPushButton;
    buttonPrev->setObjectName("buttonPrev");
    buttonPrev->setToolTip(tr("Previous image"));
    buttonPrev->setIcon(QIcon(":/tools/prev-img"));
    connect(buttonPrev, SIGNAL(clicked(bool)), _context, SLOT(gotoPrevResult()));

    auto buttonNext = new QPushButton;
    buttonNext->setObjectName("buttonNext");
    buttonNext->setToolTip(tr("Next image"));
    buttonNext->setIcon(QIcon(":/tools/next-img"));
    connect(buttonNext, SIGNAL(clicked(bool)), _context, SLOT(gotoNextResult()));

    auto buttonLast = new QPushButton;
    buttonLast->setObjectName("buttonLast");
    buttonLast->setToolTip(tr("Last image"));
    buttonLast->setIcon(QIcon(":/tools/last-img"));
    connect(buttonLast, SIGNAL(clicked(bool)), _context, SLOT(gotoLastResult()));

    _infoNav = new QLabel;
    _infoNav->setAlignment(Qt::AlignTop | Qt::AlignRight);
    _infoNav->setProperty("qss-role", "link");

    auto navLayout = Ori::Gui::layoutH({buttonFirst, 0, buttonPrev, 0, buttonNext, 0, buttonLast});
    _panelNav = makePanel({
        Ori::Gui::layoutH({ Ori::Gui::makeTitle("NAVIGATION"), 0, _infoNav }),
        navLayout
    });

    setLayout(Ori::Gui::layoutV(0, 0,
        { panelCounters, _panelPrecision, _panelZoom, _panelNav, _panelMetrics, _panelWorstPrediction, 0 }));

    resetInfo();
    updateOnModeChanged(AppConfig::currentMode().value<Mode>());
    updateOnEffectiveZoomChanged(AppConfig::zoom());
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

void ResultsPanel::currentResultChanged(int currentResult, int resultCount) {
    _infoNav->setText(QString("<span style='color:#969C9E'>%1 / %2</span>").arg(currentResult + 1).arg(resultCount));
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
    _infoNav->setText("");
}

void ResultsPanel::updateOnModeChanged(Mode mode) {
    bool v = mode.type == Mode::Type::CLASSIFICATION;
    _panelMetrics->setVisible(v);
    _panelWorstPrediction->setVisible(v);
    _panelPrecision->setVisible(!v);
    _panelZoom->setVisible(!v);
    _panelNav->setVisible(!v);
}

void ResultsPanel::updateOnEffectiveZoomChanged(double z) {
    int p = z * 100;
    _infoZoom->setText(QString("<span style='color:#969C9E'>%1%</span>").arg(p));
}
