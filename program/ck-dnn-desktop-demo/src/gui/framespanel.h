#ifndef FRAMESPANEL_H
#define FRAMESPANEL_H

#include <QFrame>
#include <QList>
#include <QThread>

#include "appmodels.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
QT_END_NAMESPACE

class ExperimentContext;
class ExperimentProbe;
class FrameWidget;
class Recognizer;

//-----------------------------------------------------------------------------

class BatchItem : public QThread
{
    Q_OBJECT
public:
    BatchItem(int index, Recognizer *recognizer, ImagesBank* images);
    ~BatchItem();

    FrameWidget* frame() const { return _frame; }

    void run() override;
    void runIteration();

signals:
    void finished(const ExperimentProbe* probe);
    void stopped();

private:
    Recognizer* _recognizer;
    FrameWidget* _frame;
    ExperimentProbe _probe;
    ImagesBank* _images;
    int _index;
};

//-----------------------------------------------------------------------------

class BatchSeries : public QThread
{
    Q_OBJECT
public:
    BatchSeries(QObject* parent) : QThread(parent) {}

    void run() override;

    void setItems(QList<BatchItem*> items) { _batchItems = items; }

signals:
    void finished();

private:
    QList<BatchItem*> _batchItems;
};

//-----------------------------------------------------------------------------

class FramesPanel : public QFrame
{
    Q_OBJECT

public:
    explicit FramesPanel(ExperimentContext* context, QWidget *parent = 0);
    ~FramesPanel();

private slots:
    void experimentStarted();
    void experimentStopping();
    void batchStopped();
    void experimentFinished();

private:
    Recognizer* _recognizer = nullptr;
    ExperimentContext* _context;
    QList<BatchItem*> _batchItems;
    bool _experimentFinished = true;
    bool _runInParallel = false;
    BatchSeries* _series = nullptr;
    QGridLayout* _layout;
    ImagesBank* _images = nullptr;

    void clearBatch();
    void prepareBatch();
    QString canStart();
    void abortExperiment(const QString &errorMsg = QString());
    void releaseExperiment();
};

#endif // FRAMESPANEL_H
