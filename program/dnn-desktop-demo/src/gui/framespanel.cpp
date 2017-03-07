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

BatchItem::BatchItem(int index, Recognizer *recognizer, ImagesBank *images) : QThread(0)
{
    _index = index;
    _images = images;
    _recognizer = recognizer;
    _frame = new FrameWidget;
    _probe.predictions.resize(_recognizer->predictionsCount());
}

BatchItem::~BatchItem()
{
    delete _frame;
}

int batchesProcessed = 0;

void BatchItem::run()
{
    while (!isInterruptionRequested())
    {
        runIteration();
    }
    qDebug() << "Batch item stopped" << _index;
    emit stopped();
}

void BatchItem::runIteration()
{
    int imageIndex = qrand() % _images->images().size();
    auto image = _images->images().at(imageIndex);
    _recognizer->recognize(image, _probe);
    _frame->loadImage(image.fileName, _probe.correctInfo);
    _frame->showPredictions(_probe.predictions);
    emit finished(&_probe);
}

//-----------------------------------------------------------------------------

void BatchSeries::run()
{
    int i = 0;
    int count = _batchItems.size();
    while (!isInterruptionRequested())
        _batchItems.at(i++ % count)->runIteration();
    emit finished();
}

//-----------------------------------------------------------------------------

FramesPanel::FramesPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
//    _runInParallel = false; // TODO

    setObjectName("framesPanel");

    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentStopping()), this, SLOT(experimentStopping()));

//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

//    if (!_runInParallel)
//    {
//        _series = new BatchSeries(this);
//        connect(_series, &BatchSeries::finished, this, &FramesPanel::experimentFinished);
//    }

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

//    clearBatch();
//    releaseExperiment();
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


//    const DnnModel& model = _context->models().current();
//    const DnnEngine& engine = _context->engines().current();
//    const ImagesDataset& images = _context->images().current();

//    _images = new ImagesBank(images.imagesPath(), images.valFile());
//    if (_images->isEmpty())
//        return abortExperiment("No images for processing");

//    qDebug() << "----------------------------------------------";
//    qDebug() << "Start experiment for" << engine.title()
//             << "with" << model.title()
//             << "on" << images.title();

//    _recognizer = new Recognizer(engine.library(), engine.deps());
//    if (!_recognizer->ready())
//        return abortExperiment();

//    if (!_recognizer->prepare(model.modelFile(), model.weightsFile(),
//                              images.meanFile(), images.labelsFile()))
//        return abortExperiment();

//    clearBatch();
//    prepareBatch();

//    qDebug() << "Start batch processing";
//    _experimentFinished = false;
//    if (_series)
//        _series->start();
//    else
//        for (auto item: _batchItems)
//            item->start();
}

void FramesPanel::experimentStopping()
{
    qDebug() << "Stopping batch processing";
    if (nullptr != _worker) {
        _worker->requestInterruption();
    }
//    if (_series)
//        _series->requestInterruption();
//    else
//        for (auto item: _batchItems)
//            item->requestInterruption();
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
    //releaseExperiment();
}

//void FramesPanel::clearBatch()
//{
//    for (auto item: _batchItems)
//        delete item;
//    _batchItems.clear();
//}

//void FramesPanel::prepareBatch()
//{
//    qDebug() << "Prepare batch items";
//    const int framesCount = 8;
//    for (int i = 0; i < framesCount; i++)
//    {
//        auto item = new BatchItem(i, _recognizer, _images);
//        connect(item, &BatchItem::stopped, this, &FramesPanel::batchStopped);
//        connect(item, &BatchItem::finished, _context, &ExperimentContext::recognitionFinished);
//        _batchItems.append(item);
//        int row = (i > framesCount/2-1)? 1: 0;
//        int col = ((i - framesCount/2) < 0)? i: i-framesCount/2;
//        _layout->addWidget(item->frame(), row, col);

//    }
//    if (_series)
//        _series->setItems(_batchItems);
//}

//void FramesPanel::batchStopped()
//{
//    if (nullptr == _worker) return;

//    for (auto item: _batchItems)
//        if (!item->isFinished())
//            return;

//    experimentFinished();
//}

//void FramesPanel::experimentFinished()
//{
//    qDebug() << "Batch processing finished";
//    emit _context->experimentFinished();
//    _experimentFinished = true;
//    releaseExperiment();
//    clearWorker();
//}

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

//void FramesPanel::releaseExperiment() {
//    if (_recognizer)
//    {
//        delete _recognizer;
//        _recognizer = nullptr;
//    }
//    if (_images)
//    {
//        delete _images;
//        _images = nullptr;
//    }
//}
