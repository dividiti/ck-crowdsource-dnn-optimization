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
    connect(_context, &ExperimentContext::experimentStarted, this, &FramesPanel::experimentStarted);
    connect(_context, &ExperimentContext::experimentStopping, this, &FramesPanel::experimentStopping);
    connect(_context, &ExperimentContext::currentResultChanged, this, &FramesPanel::currentResultChanged);
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
    _widgets_init = false;
    _current_result = -1;
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
        _rec_widget = new RecognitionWidget(_context);

        QVBoxLayout* l = new QVBoxLayout;
        _layout = l;
        l->addWidget(_rec_widget);
        setLayout(l);
    }
    _widgets_init = true;
}

static QString cannotRunError() {
    if (!AppConfig::currentProgram().isValid() || !AppConfig::currentModel().isValid() || !AppConfig::currentDataset().isValid()) {
        return "Please select engine, model and dataset first";
    }
    return "";
}

void FramesPanel::experimentStarted(bool resume) {
    if (Q_NULLPTR != _worker) {
        return abortExperiment("Another experiment is already running");
    }
    QString errorMsg = cannotRunError();
    if (errorMsg.isEmpty()) {
        Mode mode = AppConfig::currentMode().value<Mode>();
        _worker = new WorkerThread(AppConfig::currentProgram().value<Program>(), mode, this);
        _worker->setModel(AppConfig::currentModel().value<Model>());
        _worker->setDataset(AppConfig::currentDataset().value<Dataset>());
        if (mode.type == Mode::Type::CLASSIFICATION) {
            _worker->setBatchSize(AppConfig::batchSize());
        } else {
            _worker->setMinResultInterval(AppConfig::recognitionUpdateIntervalMs());
            if (resume) {
                _worker->setSkipFilesIncluding(_context->lastResult().originalImageFile);
            }
        }
        if (!resume || !_widgets_init) {
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

void FramesPanel::currentResultChanged(int index, int, ImageResult ir) {
    if (Q_NULLPTR == _rec_widget) {
        return;
    }
    if (_current_result != index) {
        _rec_widget->load(ir);
        _current_result = index;
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
    }
}

void FramesPanel::workerStopped() {
    qDebug() << "Batch processing finished";
    bool normalExit = _worker->isNormalExit();
    clearWorker();
    emit _context->experimentFinished(normalExit);
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
