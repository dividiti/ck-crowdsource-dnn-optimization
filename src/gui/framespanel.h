#ifndef FRAMESPANEL_H
#define FRAMESPANEL_H

#include <QFrame>
#include <QList>
#include <QThread>

#include "appmodels.h"

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
    BatchItem(int index, int imageOffset, Recognizer *recognizer, ImagesBank* images);
    ~BatchItem();

    FrameWidget* frame() const { return _frame; }

    void run() override;

signals:
    void finished(const ExperimentProbe* probe);
    void stopped();

private:
    ImagesBank* _images;
    Recognizer* _recognizer;
    FrameWidget* _frame;
    ExperimentProbe _probe;
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
    Recognizer* _recognizer = nullptr;
    ImagesBank* _images = nullptr;
    ExperimentContext* _context;
    QList<BatchItem*> _batchItems;
    bool _experimentFinished = false;

    void clearBatch();
    void prepareBatch();
    bool prepareImages();
    bool allItemsStopped();
    QString canStart();
};

#endif // FRAMESPANEL_H
