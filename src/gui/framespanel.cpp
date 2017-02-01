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
#include <QTimer>

ImagesBank::ImagesBank(const QString& imagesDir)
{
    //auto imagesDir = AppConfig::imagesDir();
    qDebug() << "Images directory:" << imagesDir;

    auto imagesFilter = AppConfig::imagesFilter();
    qDebug() << "Images filter:" << imagesFilter.join(",");

    QDir dir(imagesDir);
    for (const QString& entry: dir.entryList(imagesFilter, QDir::Files))
        _images << imagesDir + QDir::separator() + entry;

    qDebug() << "Images found:" << _images.size();
}

//-----------------------------------------------------------------------------

QString OutputParser::parse(ExperimentProbe &probe, const QString& text)
{
    static QLatin1String predictionMarker("0."); // 0.9835 - "n03793489 mouse, computer mouse"
    static QLatin1String timeMarker("\"execution_time\":"); // "execution_time": 0.244448,
    static QLatin1String errorMarker("CK error:");

    for (const QStringRef& line: text.splitRef('\n', QString::SkipEmptyParts))
    {
        QStringRef s = line.trimmed();
        if (s.startsWith(predictionMarker))
            parsePredictionLine(probe, s);
        else if (s.startsWith(timeMarker))
            parseTimeLine(probe, s);
        else if (s.startsWith(errorMarker))
            return s.toString();
    }

    return QString();
}

void OutputParser::parsePredictionLine(ExperimentProbe &probe, const QStringRef line)
{
    // 0.9835 - "n03793489 mouse, computer mouse"
    auto probAndDescr = line.split('-');
    if (probAndDescr.size() < 2) return;
    auto prob =  probAndDescr.at(0).trimmed();
    auto descr = probAndDescr.at(1).trimmed();

    // trim quotes
    descr = descr.left(descr.length()-1).right(descr.length()-2);
    auto spacePos = descr.indexOf(' ');
    auto id = spacePos > 0? descr.left(spacePos): QStringRef();
    descr = descr.right(descr.length()-spacePos-1);

    PredictionResult prediction;
    prediction.probability = prob.toDouble();
    prediction.description = descr.toString();
    prediction.id = id.toString();
    probe.predictions << prediction;
}

void OutputParser::parseTimeLine(ExperimentProbe &probe, const QStringRef line)
{
    // "execution_time": 0.244448,
    auto nameAndValue = line.split(":");
    if (nameAndValue.size() < 2) return;
    QStringRef value = nameAndValue.at(1).trimmed();
    probe.time = value.left(value.size()-1).toDouble();
}

//-----------------------------------------------------------------------------

BatchItem::BatchItem(int index, int imageOffset, const ScenarioRunParams& params, ImagesBank *images) : QObject(0)
{
    _index = index;
    _images = images;
    _imageIndex = imageOffset;
    _frame = new FrameWidget;
    _runner = new ScenarioRunner(params, _index);
    connect(_runner, &ScenarioRunner::scenarioFinished, this, &BatchItem::scenarioFinished);
}

BatchItem::~BatchItem()
{
    delete _frame;
    delete _runner;
}

void BatchItem::run()
{
    _isStopped = false;
    _stopFlag = false;
    runInternal();
}

void BatchItem::runInternal()
{
    _runner->run(_images->imageFile(_imageIndex));
}

void BatchItem::scenarioFinished(const QString &error)
{
    if (error.isEmpty())
    {
        auto imageFile = _images->imageFile(_imageIndex);
        _frame->loadImage(imageFile);
        ExperimentProbe p;
        auto err = OutputParser::parse(p, _runner->getStdout());
        if (err.isEmpty())
        {
            _frame->showPredictions(p.predictions);
            p.image = imageFile;
            emit finished(p);
        }
        else
        {
            _stopFlag = true;
            AppEvents::error(QString("Frame %1:\n%2").arg(_index).arg(err));
        }
    }
    else
    {
        _stopFlag = true;
        AppEvents::error(QString("Frame %1:\n%2").arg(_index).arg(error));
    }

    if (_stopFlag)
    {
        stopInternal();
        return;
    }

    _imageIndex++;
    if (_imageIndex == _images->size())
        _imageIndex = 0;

    if (nextBatch)
        QTimer::singleShot(100, nextBatch, &BatchItem::runInternal);
    else runInternal();
}

void BatchItem::stopInternal()
{
    qDebug() << "Batch item stopped" << _index;
    _isStopped = true;
    emit stopped();

    if (nextBatch && !nextBatch->_isStopped)
        QTimer::singleShot(10, nextBatch, &BatchItem::stopInternal);
}

//-----------------------------------------------------------------------------

FramesPanel::FramesPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
    setObjectName("framesPanel");

    _runParallel = AppConfig::isParallel();
    qDebug() << "Run in parallel:" << _runParallel;

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
    auto model = _context->models().current();
    qDebug() << "Start experiment for" << engine.title() << "on" << model.title();

    ScenarioRunParams params(engine, model);

    clearBatch();
    prepareBatch(params);

    qDebug() << "Start batch processing";
    if (_runParallel)
        for (auto item: _batchItems)
            item->run();
    else
        _batchItems.at(0)->run();
}

void FramesPanel::experimentStopping()
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
        int offset = qRound(_images->size()/double(_context->batchSize())*i);
        auto item = new BatchItem(i, offset, params, _images);
        connect(item, &BatchItem::stopped, this, &FramesPanel::batchStopped);
        connect(item, &BatchItem::finished, _context, &ExperimentContext::recognitionFinished);
        _batchItems.append(item);
        layout()->addWidget(item->frame());
    }
    if (!_runParallel)
    {
        for (int i = 0; i < _context->batchSize()-1; i++)
            _batchItems.at(i)->nextBatch = _batchItems.at(i+1);
        _batchItems.at(_context->batchSize()-1)->nextBatch = _batchItems.at(0);
    }
}

bool FramesPanel::prepareImages()
{
    qDebug() << "Prepare images";
    if (!_images)
        _images = new ImagesBank(_context->images().current().imagesPath());
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
        if (!item->isStopped())
            return false;
    return true;
}

void FramesPanel::batchStopped()
{
    if (allItemsStopped())
    {
        qDebug() << "Batch processing finished";
        emit _context->experimentFinished();
    }
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
