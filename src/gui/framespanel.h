#ifndef FRAMESPANEL_H
#define FRAMESPANEL_H

#include <QWidget>
#include <QList>

class ExperimentContext;
class FrameWidget;
class ScenarioRunner;
class ScenarioRunParams;

class ImagesBank
{
public:
    ImagesBank();

    int size() const { return _images.size(); }
    QString imageFile(int index) const { return _images.at(index); }

private:
    QStringList _images;
};

//-----------------------------------------------------------------------------

class BatchItem : public QObject
{
    Q_OBJECT
public:
    BatchItem(int index, const ScenarioRunParams& params, ImagesBank* images);
    ~BatchItem();

    FrameWidget* frame() const { return _frame; }

    void run();
    void stop() { _stopFlag = true; }
    bool isFnished() const { return _isFnished; }

signals:
    void finished();

private slots:
    void scenarioFinished(const QString &error);

private:
    ImagesBank* _images;
    ScenarioRunner* _runner;
    FrameWidget* _frame;
    bool _stopFlag = false;
    bool _isFnished = false;
    int _index;
    int _imageIndex;

    void runInternal();
};

//-----------------------------------------------------------------------------

class FramesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FramesPanel(ExperimentContext* context, QWidget *parent = 0);
    ~FramesPanel();

private slots:
    void experimentStarted();
    void experimentStopped();
    void batchFinished();

private:
    ImagesBank* _images = nullptr;
    ExperimentContext* _context;
    QList<BatchItem*> _batchItems;

    void clearBatch();
    void prepareBatch(const ScenarioRunParams &params);
    bool prepareImages();
    bool allItemsFinished();
};

#endif // FRAMESPANEL_H
