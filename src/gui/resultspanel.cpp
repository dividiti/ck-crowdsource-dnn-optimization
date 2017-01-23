#include "infolabel.h"
#include "resultspanel.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QBoxLayout>
#include <QPushButton>

ResultsPanel::ResultsPanel(QWidget *parent) : QWidget(parent)
{
    auto infoTimePerImage = new InfoLabel(tr("Time per image\n(FPS):"));
    auto infoTimePerBatch = new InfoLabel(tr("Time per batch:"));
    auto infoMemoryUsage = new InfoLabel(tr("Memory usage\nper image:"));
    infoTimePerImage->setInfo("N/A");
    infoTimePerBatch->setInfo("N/A");
    infoMemoryUsage->setInfo("N/A");

    auto buttonPublish = new QPushButton(tr("Publish"));
    buttonPublish->setEnabled(false);

    setLayout(Ori::Gui::layoutV(0, 3*Ori::Gui::layoutSpacing(),
    {
        infoTimePerImage,
        Utils::makeDivider(),
        infoTimePerBatch,
        Utils::makeDivider(),
        infoMemoryUsage,
        0,
        buttonPublish,
    }));
}
