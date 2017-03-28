#ifndef FRAMESPANEL_H
#define FRAMESPANEL_H

#include <QFrame>
#include <QList>
#include <QThread>

#include "appmodels.h"
#include "workerthread.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
QT_END_NAMESPACE

class ExperimentContext;
class FrameWidget;

class FramesPanel : public QFrame
{
    Q_OBJECT

public:
    explicit FramesPanel(ExperimentContext* context, QWidget *parent = 0);
    ~FramesPanel();

private slots:
    void experimentStarted();
    void experimentStopping();

    void newImageResult(ImageResult);
    void workerStopped();

private:
    ExperimentContext* _context;
    QGridLayout* _layout;
    WorkerThread* _worker = Q_NULLPTR;
    Mode _mode;

    QList<FrameWidget*> _frames;
    const int _frame_count = 8;
    int _current_frame = 0;

    void abortExperiment(const QString &errorMsg = QString());
    void releaseExperiment();

    void clearWorker();
};

#endif // FRAMESPANEL_H
