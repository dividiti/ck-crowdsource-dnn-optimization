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

ImagesBank::ImagesBank(const QString& imagesDir)
{
    qDebug() << "Images directory:" << imagesDir;

    auto imagesFilter = AppConfig::imagesFilter();
    qDebug() << "Images filter:" << imagesFilter.join(",");

    QDir dir(imagesDir);
    for (const QString& entry: dir.entryList(imagesFilter, QDir::Files))
        _images << imagesDir + QDir::separator() + entry;

    qDebug() << "Images found:" << _images.size();
}

//-----------------------------------------------------------------------------

BatchItem::BatchItem(int index, int imageOffset, Recognizer *recognizer, ImagesBank *images) : QThread(0)
{
    _index = index;
    _images = images;
    _imageIndex = imageOffset;
    _recognizer = recognizer;
    _frame = new FrameWidget;
    _probe.predictions.resize(_recognizer->predictionsCount());
}

BatchItem::~BatchItem()
{
    delete _frame;
}

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
    _imageIndex = qrand() % _images->size();

    auto imageFile = _images->imageFile(_imageIndex);
    _recognizer->recognize(imageFile, _probe);
    qDebug() << "1";

    // TODO: after "1" after some ten iterations: Segmentation fault

    _frame->loadImage(imageFile);
    qDebug() << "2";
    _frame->showPredictions(_probe.predictions);
    qDebug() << "3";
    emit finished(&_probe);
    qDebug() << "4";

//    _imageIndex++;
//    if (_imageIndex == _images->size())
//        _imageIndex = 0;
}

//-----------------------------------------------------------------------------

void BatchSeries::run()
{
    while (!isInterruptionRequested())
    {
        for (auto batch: _batchItems)
        {
            batch->runIteration();
            if (isInterruptionRequested())
                break;
        }
    }
    emit finished();
}

//-----------------------------------------------------------------------------

FramesPanel::FramesPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
    _runInParallel = false; // TODO

    setObjectName("framesPanel");

    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentStopping()), this, SLOT(experimentStopping()));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    if (!_runInParallel)
    {
        _series = new BatchSeries(this);
        connect(_series, &BatchSeries::finished, this, &FramesPanel::experimentFinished);
    }

    _layout = new QGridLayout;
    _layout->setSpacing(16);
    _layout->setMargin(0);
    setLayout(_layout);
}

FramesPanel::~FramesPanel()
{
    clearBatch();
    if (_images) delete _images;
    if (_recognizer) delete _recognizer;
}

void FramesPanel::experimentStarted()
{
    auto res = canStart();
    if (!res.isEmpty())
    {
        AppEvents::error(res);
        QTimer::singleShot(200, _context, SIGNAL(experimentFinished()));
        return;
    }

    if (!prepareImages())
    {
        QTimer::singleShot(200, _context, SIGNAL(experimentFinished()));
        return;
    }

    auto engine = _context->engines().current();
    //auto model = _context->models().current();
    qDebug() << "Start experiment for" << engine.title();// << "on" << model.title();

    _recognizer = new Recognizer(engine.library(), engine.paths());
    if (!_recognizer->ready())
    {
        QTimer::singleShot(200, _context, SIGNAL(experimentFinished()));
        delete _recognizer;
        _recognizer = nullptr;
        return;
    }
    // TODO: prepare recognizer with real data
    QString model("/home/kolyan/CK/ck-caffe/program/caffe-classification/tmp/tmp-BVwvo7.prototxt");
    QString weights("/home/kolyan/CK-TOOLS/caffemodel-bvlc-googlenet/bvlc_googlenet.caffemodel");
    QString mean("/home/kolyan/CK/ck-caffe/program/caffe-classification/imagenet_mean.binaryproto");
    QString labels("/home/kolyan/CK/ck-caffe/program/caffe-classification/synset_words.txt");
    _recognizer->prepare(model, weights, mean, labels);

    clearBatch();
    prepareBatch();

    qDebug() << "Start batch processing";
    _experimentFinished = false;
    if (_series)
        _series->start();
    else
        for (auto item: _batchItems)
            item->start();
}

void FramesPanel::experimentStopping()
{
    qDebug() << "Stopping batch processing";
    if (_series)
        _series->requestInterruption();
    else
        for (auto item: _batchItems)
            item->requestInterruption();
}

void FramesPanel::clearBatch()
{
    for (auto item: _batchItems)
        delete item;
    _batchItems.clear();
}

void FramesPanel::prepareBatch()
{
    qDebug() << "Prepare batch items";
    //int framesCount = _context->batchSize();
    int framesCount = 8;
    for (int i = 0; i < framesCount; i++)
    {
        int offset = qRound(_images->size()/double(framesCount)*i);
        auto item = new BatchItem(i, offset, _recognizer, _images);
        connect(item, &BatchItem::stopped, this, &FramesPanel::batchStopped);
        connect(item, &BatchItem::finished, _context, &ExperimentContext::recognitionFinished);
        _batchItems.append(item);
        int row = (i > framesCount/2-1)? 1: 0;
        int col = ((i - framesCount/2) < 0)? i: i-framesCount/2;
        _layout->addWidget(item->frame(), row, col);

    }
    if (_series)
        _series->setItem(_batchItems);
}

bool FramesPanel::prepareImages()
{
    qDebug() << "Prepare images";
    if (!_images)
    {
        auto imagesDir = AppConfig::imagesDir();
        //auto imagesDir = _context->images().current().imagesPath();
        _images = new ImagesBank(imagesDir);
    }
    if (_images->size() < 1)
    {
        AppEvents::error(tr("No images for processing"));
        return false;
    }
    return true;
}

void FramesPanel::batchStopped()
{
    if (_experimentFinished) return;

    for (auto item: _batchItems)
        if (!item->isFinished())
            return;

    experimentFinished();
}

void FramesPanel::experimentFinished()
{
    qDebug() << "Batch processing finished";
    emit _context->experimentFinished();
    _experimentFinished = true;
    delete _recognizer;
    _recognizer = nullptr;
}

QString FramesPanel::canStart()
{
    if (!_context->engines().hasCurrent())
        return tr("No engine selected");

//    if (!_context->models().hasCurrent())
//        return tr("No scenario selected");

//    if (!_context->images().hasCurrent())
//        return tr("No image source selected");

    return QString();
}
