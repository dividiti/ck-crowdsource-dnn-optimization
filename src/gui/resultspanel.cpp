#include "infolabel.h"
#include "experimentcontext.h"
#include "resultspanel.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>

ResultsPanel::ResultsPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
    setObjectName("resultsPanel");

    _context = context;
    connect(_context, &ExperimentContext::experimentStarted, this, &ResultsPanel::experimentStarted);
    connect(_context, &ExperimentContext::experimentResultReady, this, &ResultsPanel::experimentResultReady);

    _infoTimePerFrame = new InfoLabel(tr("TIME PER\nIMAGE (FPS)"));
    _infoTimePerBatch = new InfoLabel(tr("Time per batch:"));
    _infoMemoryUsage = new InfoLabel(tr("Memory usage\nper image:"));

    setLayout(Ori::Gui::layoutV(0, 3*Ori::Gui::layoutSpacing(),
    {
        _infoTimePerFrame,
        /*Utils::makeDivider(),
        _infoTimePerBatch,
        Utils::makeDivider(),
        _infoMemoryUsage,*/
        0,
    }));
}

void ResultsPanel::experimentStarted()
{
    _infoTimePerFrame->setInfo("N/A");
    _infoTimePerBatch->setInfo("N/A");
    _infoMemoryUsage->setInfo("N/A");
}

void ResultsPanel::experimentResultReady()
{
    auto r = _context->experimentResult();
    _infoTimePerFrame->setInfo(QString("%1\n(%2)").arg(r.timePerImage).arg(r.imagesPerSecond));
    _infoTimePerBatch->setInfo(QString::number(r.timePerBatch));
    _infoMemoryUsage->setInfo(QString::number(r.memoryPerImage));
}
