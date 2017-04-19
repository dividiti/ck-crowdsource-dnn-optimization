#include "appconfig.h"
#include "appevents.h"
#include "experimentcontext.h"
#include "framespanel.h"
#include "framewidget.h"
#include "recognitionwidget.h"

#include <QBoxLayout>
#include <QDebug>
#include <QGridLayout>
#include <QTimer>

FramesPanel::FramesPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent) {
    setObjectName("framesPanel");

    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentStopping()), this, SLOT(experimentStopping()));
}

FramesPanel::~FramesPanel() {
    clearWorker();
}

void FramesPanel::clearWidgets() {
    delete _layout;
    for (auto f : _frames) {
        delete f;
    }
    _frames.clear();
    delete _rec_widget;
    _rec_widget = Q_NULLPTR;
}

void FramesPanel::initLayout() {
    if (!_worker) {
        return;
    }
    clearWidgets();
    if (_worker->getMode().type == Mode::Type::CLASSIFICATION) {
        QGridLayout* l = new QGridLayout;
        _layout = l;
        l->setSpacing(24);
        l->setMargin(0);
        setLayout(l);

        const int framesCount = _frame_count;
        for (int i = 0; i < framesCount; i++) {
            FrameWidget* f = new FrameWidget;
            _frames.append(f);
            int row = (i > framesCount/2-1)? 1: 0;
            int col = ((i - framesCount/2) < 0)? i: i-framesCount/2;
            l->addWidget(f, row, col);
        }
    } else {
        _rec_widget = new RecognitionWidget;

        QVBoxLayout* l = new QVBoxLayout;
        _layout = l;
        l->addStretch();
        l->addWidget(_rec_widget);
        l->addStretch();
        setLayout(l);
    }
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
            _worker = new WorkerThread(AppConfig::currentProgram().value<Program>(), mode, this);
            _worker->setModel(AppConfig::currentModel().value<Model>());
            _worker->setDataset(AppConfig::currentDataset().value<Dataset>());
            _worker->setBatchSize(AppConfig::batchSize());
            initLayout();
        } else {
            _worker = new WorkerThread(AppConfig::currentSqueezeDetProgram().value<Program>(), mode, this);
            _worker->setMinResultInterval(AppConfig::recognitionUpdateIntervalMs());
            initLayout();
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
    if (Q_NULLPTR == _worker) {
        return;
    }
    if (_worker->getMode().type == Mode::Type::CLASSIFICATION) {
        FrameWidget* f = _frames[_current_frame];
        f->load(ir);
        _current_frame = (_current_frame + 1) % _frame_count;
    } else {
        _rec_widget->load(ir);
    }
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
