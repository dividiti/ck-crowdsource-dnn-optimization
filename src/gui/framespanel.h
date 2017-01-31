#ifndef FRAMESPANEL_H
#define FRAMESPANEL_H

#include <QFrame>
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

class OutputParser
{
public:
    static QString parse(ExperimentProbe& probe, const QString& text);
private:
    static void parsePredictionLine(ExperimentProbe &probe, const QStringRef line);
    static void parseTimeLine(ExperimentProbe &probe, const QStringRef line);
};

//-----------------------------------------------------------------------------

class BatchItem : public QObject
{
    Q_OBJECT
public:
    BatchItem(int index, int imageOffset, const ScenarioRunParams& params, ImagesBank* images);
    ~BatchItem();

    BatchItem* nextBatch = nullptr;

    FrameWidget* frame() const { return _frame; }

    void run();
    void stop() { _stopFlag = true; }
    bool isStopped() const { return _isStopped; }

signals:
    void finished(const ExperimentProbe& probe);
    void stopped();

private slots:
    void runInternal();
    void stopInternal();
    void scenarioFinished(const QString &error);

private:
    ImagesBank* _images;
    ScenarioRunner* _runner;
    FrameWidget* _frame;
    bool _stopFlag = false;
    bool _isStopped = false;
    int _index, _imageIndex;
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

private:
    ImagesBank* _images = nullptr;
    ExperimentContext* _context;
    QList<BatchItem*> _batchItems;
    bool _runParallel;

    void clearBatch();
    void prepareBatch(const ScenarioRunParams &params);
    bool prepareImages();
    bool allItemsStopped();
    QString canStart();
};

#endif // FRAMESPANEL_H
