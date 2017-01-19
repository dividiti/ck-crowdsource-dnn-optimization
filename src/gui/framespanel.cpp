#include "framespanel.h"
#include "framewidget.h"

#include <QBoxLayout>

FramesPanel::FramesPanel(QWidget *parent) : QScrollArea(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto content = new QWidget;
    content->setLayout(new QHBoxLayout);
    content->layout()->setSpacing(0);
    content->layout()->setMargin(0);
    for (int i = 0; i < 16; i++)
        content->layout()->addWidget(new FrameWidget);

    setWidget(content);
}
