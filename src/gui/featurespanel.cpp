#include "featurespanel.h"
#include "infolabel.h"

#include <QBoxLayout>

FeaturesPanel::FeaturesPanel(QWidget *parent) : QWidget(parent)
{
    auto infoPlatform = new InfoLabel(tr("Platform:"));
    auto infoEngine = new InfoLabel(tr("Engine:"));
    auto infoBatchSize = new InfoLabel(tr("Batch size:"));
    infoPlatform->setInfo("This machine");
    infoEngine->setInfo("AlexNet / CPU");
    infoBatchSize->setInfo("2");

    auto layout = new QVBoxLayout;
    layout->addWidget(infoPlatform);
    layout->addWidget(infoEngine);
    layout->addWidget(infoBatchSize);
    layout->addStretch();
    setLayout(layout);
}
