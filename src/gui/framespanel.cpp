#include "appconfig.h"
#include "appevents.h"
#include "experimentcontext.h"
#include "framespanel.h"
#include "framewidget.h"
#include "scenariorunner.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDir>

ImagesBank::ImagesBank()
{
    auto imagesDir = AppConfig::imagesDir();
    qDebug() << "Images directory:" << imagesDir;

    auto imagesFilter = AppConfig::imagesFilter();
    qDebug() << "Images filter:" << imagesFilter.join(";");

    QDir dir(imagesDir);
    for (const QString& entry: dir.entryList(imagesFilter, QDir::Files))
        _images << imagesDir + QDir::separator() + entry;

    qDebug() << "Images found:" << _images.size();
}

//-----------------------------------------------------------------------------

BatchItem::BatchItem(int index, const ScenarioRunParams& params, ImagesBank *images) : QObject(0)
{
    _index = index;
    _images = images;
    _imageIndex = 0; // TODO: offset
    _frame = new FrameWidget;
    _runner = new ScenarioRunner(params);
    connect(_runner, &ScenarioRunner::scenarioFinished, this, &BatchItem::scenarioFinished);
}

BatchItem::~BatchItem()
{
    delete _frame;
    delete _runner;
}

void BatchItem::run()
{
    _isFnished = false;
    runInternal();
}

void BatchItem::runInternal()
{
    auto image = _images->imageFile(_imageIndex);
    _runner->run(image);
    _frame->loadImage(image);
}

void BatchItem::scenarioFinished(const QString &error)
{
    if (!error.isEmpty())
        return AppEvents::error(error);

    // TODO: parse info
    _frame->showInfo(_runner->stdout());

    // TODO: read timers json
    // TODO: calculate times

    if (_stopFlag)
    {
        qDebug() << "Batch item stopped" << _index;
        _isFnished = true;
        emit finished();
        return;
    }

    _imageIndex++;
    if (_imageIndex == _images->size())
        _imageIndex = 0;
    run();
}

//-----------------------------------------------------------------------------

FramesPanel::FramesPanel(ExperimentContext *context, QWidget *parent) : QWidget(parent)
{
    _context = context;
    connect(_context, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
    connect(_context, SIGNAL(experimentStopped()), this, SLOT(experimentStopped()));

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
}

void FramesPanel::experimentStarted()
{
    if (!_context->currentScenarioExists())
        return AppEvents::error(tr("No scenario selected"));

    auto scenario = _context->currentScenario();
    qDebug() << "Start experiment for scenario" << scenario.title();

    if (!prepareImages()) return;

    ScenarioRunParams params(scenario);

    clearBatch();
    prepareBatch(params);

    qDebug() << "Start batch processing";
    for (auto item: _batchItems) item->run();
}

void FramesPanel::experimentStopped()
{
    qDebug() << "Stopping batch processing";
    for (auto item: _batchItems) item->stop();
}

void FramesPanel::clearBatch()
{
    for (auto item: _batchItems)
        delete item;
    _batchItems.clear();
}

void FramesPanel::prepareBatch(const ScenarioRunParams& params)
{
    qDebug() << "Prepare batch items";
    for (int i = 0; i < _context->batchSize(); i++)
    {
        auto item = new BatchItem(i, params, _images);
        connect(item, &BatchItem::finished, this, &FramesPanel::batchFinished);
        _batchItems.append(item);
        layout()->addWidget(item->frame());
    }
}

bool FramesPanel::prepareImages()
{
    qDebug() << "Prepare images";
    if (!_images)
        _images = new ImagesBank();
    if (_images->size() < 1)
    {
        AppEvents::error(tr("No images for processing"));
        return false;
    }
    return true;
}

bool FramesPanel::allItemsFinished()
{
    for (auto item: _batchItems)
        if (!item->isFnished())
            return false;
    return true;
}

void FramesPanel::batchFinished()
{
    if (allItemsFinished())
        emit _context->experimentFinished();
}
