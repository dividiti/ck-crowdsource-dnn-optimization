#include "experimentpanel.h"
#include "framespanel.h"
#include "featurespanel.h"
#include "resultspanel.h"

#include <QBoxLayout>

ExperimentPanel::ExperimentPanel(QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::StyledPanel);
    setLayout(new QHBoxLayout);

    layout()->addWidget(new FramesPanel);
    layout()->addWidget(new FeaturesPanel);
    layout()->addWidget(new ResultsPanel);
}
