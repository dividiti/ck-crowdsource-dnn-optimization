#include "mainwindow.h"
#include "experimentpanel.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDesktopWidget>

void setInitialWindowGeometry(QWidget* w)
{
    auto desktop = QApplication::desktop()->screen()->rect();
    w->adjustSize();
    // take some more space on the screen then auto-sized
    w->resize(desktop.width()*0.75, w->height()*1.1);
    // position at desktop center
    w->move(desktop.center() - w->rect().center());
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icon/main"));

    _experimentPanels.append(new ExperimentPanel(this));
    _experimentPanels.append(new ExperimentPanel(this));

    auto content = new QWidget;
    content->setLayout(new QVBoxLayout);
    for (auto panel: _experimentPanels)
        content->layout()->addWidget(panel);

    setCentralWidget(content);
    setInitialWindowGeometry(this);
}

MainWindow::~MainWindow()
{
}
