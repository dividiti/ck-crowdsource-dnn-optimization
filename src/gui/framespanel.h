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

class ImagesBank
{
public:
    ImagesBank(const QString &imagesDir);

    int size() const { return _images.size(); }
    const QString& imageFile(int index) const { return _images.at(index); }

private:
    QStringList _images;
};

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
    ImagesBank* _images;
    Recognizer* _recognizer;
    FrameWidget* _frame;
    ExperimentProbe _probe;
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
    ImagesBank* _images = nullptr;
    ExperimentContext* _context;
    QList<BatchItem*> _batchItems;
    bool _experimentFinished = true;
    bool _runInParallel = false;
    BatchSeries* _series = nullptr;
    QGridLayout* _layout;

    void clearBatch();
    void prepareBatch();
    bool prepareImages();
    QString canStart();
    void abortExperiment(const QString &errorMsg = QString());
};

#endif // FRAMESPANEL_H
