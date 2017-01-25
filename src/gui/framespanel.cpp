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

void FramesPanel::experimentStarted()
{
    if (!_context->currentScenarioExists())
    {
        qCritical() << "No scenario selected";
        return;
    }

    auto scenario = _context->currentScenario();
    qDebug() << "Start experiment for scenario" << scenario.title();

    prepareImagesBank();
    int imagesCount = _images.size();
    if (imagesCount < 1)
    {
        qCritical() << scenario.title() << "No images for processing";
        return;
    }

    makeFrames();

    _stopFlag = false;

    _runner = new ScenarioRunner(ScenarioRunParams(scenario));

    qDebug() << "Start batch processing";

    int imageIndex = 0;
    int batchSize = _context->batchSize();
    while (true)
    {
        for (int i = 0; i < batchSize; i++)
        {
            auto imageFile = _images.at(imageIndex);
            _frames.at(i)->loadImage(imageFile);
            _runner->run(imageFile, true);
            if (!_runner->ok())
            {
                AppEvents::error(_runner->error());
                _stopFlag = true;
                break;
            }
            else
                _frames.at(i)->showInfo(_runner->stdout());

            imageIndex++;
            if (imageIndex == imagesCount)
                imageIndex = 0;

            qApp->processEvents();

            if (_stopFlag) break;
        }
        if (_stopFlag) break;
    }

    qDebug() << "Batch processing finished";
    delete _runner;
}

void FramesPanel::experimentStopped()
{
    _stopFlag = true;
    qDebug() << "Stopping experiment...";
}

void FramesPanel::makeFrames()
{
    for (auto f: _frames)
        delete f;
    _frames.clear();

    for (int i = 0; i < _context->batchSize(); i++)
    {
        auto f = new FrameWidget;
        _frames.append(f);
        layout()->addWidget(f);
    }
}

void FramesPanel::prepareImagesBank()
{
    auto imagesDir = AppConfig::imagesDir();
    qDebug() << "Images directory" << imagesDir;

    auto imagesFilter = AppConfig::imagesFilter();
    qDebug() << "Images filter" << imagesFilter.join(";");

    _images.clear();
    QDir dir(imagesDir);
    for (const QString& entry: dir.entryList(imagesFilter, QDir::Files))
        _images << imagesDir + QDir::separator() + entry;
}

