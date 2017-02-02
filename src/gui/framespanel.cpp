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
        auto imageFile = _images->imageFile(_imageIndex);
        _recognizer->recognize(imageFile, _probe);
        msleep(qrand()%500 + 500);
        _frame->loadImage(imageFile);
        _frame->showPredictions(_probe.predictions);
        emit finished(&_probe);

        _imageIndex++;
        if (_imageIndex == _images->size())
            _imageIndex = 0;
    }
    qDebug() << "Batch item stopped" << _index;
    emit stopped();
}

//-----------------------------------------------------------------------------

FramesPanel::FramesPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
    setObjectName("framesPanel");

    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentStopping()), this, SLOT(experimentStopping()));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
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


    if (_recognizer) delete _recognizer;
    _recognizer = new Recognizer(engine.library());
    if (!_recognizer->ready())
    {
        QTimer::singleShot(200, _context, SIGNAL(experimentFinished()));
        return;
    }

    // TODO: prepare recognizer

    clearBatch();
    prepareBatch();

    qDebug() << "Start batch processing";
    _experimentFinished = false;
    for (auto item: _batchItems)
        item->start();
}

void FramesPanel::experimentStopping()
{
    qDebug() << "Stopping batch processing";
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
    for (int i = 0; i < _context->batchSize(); i++)
    {
        int offset = qRound(_images->size()/double(_context->batchSize())*i);
        auto item = new BatchItem(i, offset, _recognizer, _images);
        connect(item, &BatchItem::stopped, this, &FramesPanel::batchStopped);
        connect(item, &BatchItem::finished, _context, &ExperimentContext::recognitionFinished);
        _batchItems.append(item);
        layout()->addWidget(item->frame());
    }
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

bool FramesPanel::allItemsStopped()
{
    for (auto item: _batchItems)
        if (!item->isFinished())
            return false;
    return true;
}

void FramesPanel::batchStopped()
{
    if (!_experimentFinished && allItemsStopped())
    {
        _experimentFinished = true;
        qDebug() << "Batch processing finished";
        emit _context->experimentFinished();
    }
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
