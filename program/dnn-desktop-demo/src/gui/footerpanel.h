#ifndef FOOTERPANEL_H
#define FOOTERPANEL_H

#include "experimentcontext.h"
#include <QFrame>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class FooterPanel : public QFrame
{
    Q_OBJECT

public:
    explicit FooterPanel(ExperimentContext* context, QWidget *parent = Q_NULLPTR);

private slots:
    void publishStarted();
    void publishFinished(bool);
    void enablePublish();

private:

    ExperimentContext* _context;
    QPushButton* _buttonPublish;

};

#endif // FOOTERPANEL_H
