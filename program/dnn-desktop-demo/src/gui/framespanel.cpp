#include "appconfig.h"
#include "appevents.h"
#include "experimentcontext.h"
#include "framespanel.h"
#include "framewidget.h"
#include "recognizer.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDir>
#include <QGridLayout>
#include <QTimer>

FramesPanel::FramesPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
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

FramesPanel::~FramesPanel()
{
    clearWorker();
}

void FramesPanel::experimentStarted()
{
    auto res = canStart();
    if (!res.isEmpty()) {
        return abortExperiment(res);
    }

    if (nullptr != _worker) {
        AppEvents::error("Another experiment is already running");
        return;
    }

    initWorker();
    _current_frame = 0;
    _worker->start();
}

void FramesPanel::experimentStopping()
{
    qDebug() << "Stopping batch processing";
    if (nullptr != _worker) {
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

QString FramesPanel::canStart()
{
    if (!_context->engines().hasCurrent())
        return tr("No engine selected");

    if (!_context->models().hasCurrent())
        return tr("No scenario selected");

    if (!_context->images().hasCurrent())
        return tr("No image source selected");

    return QString();
}

void FramesPanel::initWorker() {
    if (nullptr != _worker) {
        return;
    }
    _worker = new WorkerThread();
    connect(_worker, &WorkerThread::newImageResult, this, &FramesPanel::newImageResult);
    connect(_worker, &WorkerThread::newImageResult, _context, &ExperimentContext::newImageResult);
    connect(_worker, &WorkerThread::stopped, this, &FramesPanel::workerStopped);
}

void FramesPanel::clearWorker() {
    if (nullptr == _worker) {
        return;
    }
    disconnect(_worker, &WorkerThread::newImageResult, this, &FramesPanel::newImageResult);
    disconnect(_worker, &WorkerThread::stopped, this, &FramesPanel::workerStopped);
    _worker->terminate();
    _worker->wait();
    delete _worker;
    _worker = nullptr;
}
