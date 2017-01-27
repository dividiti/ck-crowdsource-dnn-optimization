#ifndef FRAMESPANEL_H
#define FRAMESPANEL_H

#include <QWidget>
#include <QList>

class ExperimentContext;
class ExperimentProbe;
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

class StdoutResults
{
public:
    StdoutResults(const QString& text);

    QString predictions() const { return _predictions; }
    double time() const { return _time; }

private:
    QString _predictions;
    double _time = 0;
};

//-----------------------------------------------------------------------------

class BatchItem : public QObject
{
    Q_OBJECT
public:
    BatchItem(int index, int imageOffset, const ScenarioRunParams& params, ImagesBank* images);
    ~BatchItem();

    FrameWidget* frame() const { return _frame; }

    void run();
    void stop() { _stopFlag = true; }
    bool isStopped() const { return _isStopped; }

signals:
    void finished(const ExperimentProbe& probe);
    void stopped();

private slots:
    void scenarioFinished(const QString &error);

private:
    ImagesBank* _images;
    ScenarioRunner* _runner;
    FrameWidget* _frame;
    bool _stopFlag, _isStopped;
    int _index, _imageIndex;

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
    void experimentStopping();
    void batchStopped();

private:
    ImagesBank* _images = nullptr;
    ExperimentContext* _context;
    QList<BatchItem*> _batchItems;

    void clearBatch();
    void prepareBatch(const ScenarioRunParams &params);
    bool prepareImages();
    bool allItemsStopped();
};

#endif // FRAMESPANEL_H
