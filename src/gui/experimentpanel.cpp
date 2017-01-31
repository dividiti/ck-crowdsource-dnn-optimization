#include "experimentcontext.h"
#include "experimentpanel.h"
#include "framespanel.h"
#include "featurespanel.h"
#include "resultspanel.h"

#include <QBoxLayout>

ExperimentPanel::ExperimentPanel(ExperimentContext *context, QWidget *parent) : QFrame(parent)
{
    setObjectName("experimentPanel");

    auto layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(new FramesPanel(context));
    layout->addWidget(_featuresPanel = new FeaturesPanel(context));
    layout->addWidget(new ResultsPanel(context));

    setLayout(layout);
}

void ExperimentPanel::updateExperimentConditions()
{
    _featuresPanel->updateExperimentConditions();
}
