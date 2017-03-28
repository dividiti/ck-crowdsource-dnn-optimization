#include "appconfig.h"
#include "appevents.h"
#include "experimentcontext.h"
#include "framespanel.h"
#include "framewidget.h"

#include <QBoxLayout>
#include <QDebug>
#include <QGridLayout>
#include <QTimer>

FramesPanel::FramesPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent) {
    setObjectName("framesPanel");

    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentStopping()), this, SLOT(experimentStopping()));

    _layout = new QGridLayout;
    _layout->setSpacing(24);
    _layout->setMargin(0);
    setLayout(_layout);

    const int framesCount = _frame_count;
    for (int i = 0; i < framesCount; i++) {
        FrameWidget* f = new FrameWidget(this);
        _frames.append(f);
        int row = (i > framesCount/2-1)? 1: 0;
        int col = ((i - framesCount/2) < 0)? i: i-framesCount/2;
        _layout->addWidget(f, row, col);
    }
}

FramesPanel::~FramesPanel() {
    clearWorker();
}

static QString cannotRunError() {
    Mode mode = AppConfig::currentMode().value<Mode>();
    if (mode.type == Mode::Type::RECOGNITION) {
        if (!AppConfig::currentSqueezeDetProgram().isValid()) {
            return "No recognition programs found. Please, install one (e.g. SqueezeDet)";
        }
    } else {
        if (!AppConfig::currentProgram().isValid() || !AppConfig::currentModel().isValid() || !AppConfig::currentDataset().isValid()) {
            return "Please select engine, model and dataset first";
        }
    }
    return "";
}

void FramesPanel::experimentStarted() {
    if (Q_NULLPTR != _worker) {
        return abortExperiment("Another experiment is already running");
    }
    QString errorMsg = cannotRunError();
    if (errorMsg.isEmpty()) {
        Mode mode = AppConfig::currentMode().value<Mode>();
        if (mode.type == Mode::Type::CLASSIFICATION) {
            QVariant program = AppConfig::currentProgram();
            QVariant model = AppConfig::currentModel();
            QVariant dataset = AppConfig::currentDataset();
            _worker = new WorkerThread(program.value<Program>(), model.value<Model>(), dataset.value<Dataset>(), AppConfig::batchSize(),
                                       mode, this);
        } else {
            _worker = new WorkerThread(AppConfig::currentSqueezeDetProgram().value<Program>(), Model(), Dataset(), 1, mode, this);
        }
        connect(_worker, &WorkerThread::newImageResult, this, &FramesPanel::newImageResult);
        connect(_worker, &WorkerThread::newImageResult, _context, &ExperimentContext::newImageResult);
        connect(_worker, &WorkerThread::finished, this, &FramesPanel::workerStopped);
        _current_frame = 0;
        _worker->start();
    } else {
        AppEvents::error(errorMsg);
        return abortExperiment(errorMsg);
    }
}

void FramesPanel::experimentStopping() {
    qDebug() << "Stopping batch processing";
    if (Q_NULLPTR != _worker) {
        _worker->requestInterruption();
    }
}

void FramesPanel::newImageResult(ImageResult ir) {
    FrameWidget* f = _frames[_current_frame];
    f->load(ir);
    _current_frame = (_current_frame + 1) % _frame_count;
}

void FramesPanel::workerStopped() {
    qDebug() << "Batch processing finished";
    clearWorker();
    emit _context->experimentFinished();
}

void FramesPanel::abortExperiment(const QString& errorMsg) {
    if (!errorMsg.isEmpty()) {
        AppEvents::error(errorMsg);
    }
    QTimer::singleShot(200, _context, SIGNAL(experimentFinished()));
}

void FramesPanel::clearWorker() {
    if (Q_NULLPTR == _worker) {
        return;
    }
    disconnect(_worker, &WorkerThread::newImageResult, this, &FramesPanel::newImageResult);
    disconnect(_worker, &WorkerThread::newImageResult, _context, &ExperimentContext::newImageResult);
    disconnect(_worker, &WorkerThread::finished, this, &FramesPanel::workerStopped);
    _worker->deleteLater();
    _worker = Q_NULLPTR;
}
