#include "resultspanel.h"
#include "infolabel.h"

#include <QBoxLayout>
#include <QLabel>

ResultsPanel::ResultsPanel(QWidget *parent) : QWidget(parent)
{
    auto infoTimePerImage = new InfoLabel(tr("Time per image (FPS):"));
    auto infoTimePerBatch = new InfoLabel(tr("Time per batch:"));
    auto infoMemoryUsage = new InfoLabel(tr("Memory usage\nper image:"));
    infoTimePerImage->setInfo("N/A");
    infoTimePerBatch->setInfo("N/A");
    infoMemoryUsage->setInfo("N/A");

    auto layout = new QVBoxLayout;
    layout->addWidget(infoTimePerImage);
    layout->addWidget(infoTimePerBatch);
    layout->addWidget(infoMemoryUsage);
    layout->addStretch();
    setLayout(layout);
}
