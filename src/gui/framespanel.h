#ifndef FRAMESPANEL_H
#define FRAMESPANEL_H

#include <QWidget>
#include <QList>

class ExperimentContext;
class FrameWidget;
class ScenarioRunner;

class FramesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FramesPanel(ExperimentContext* context, QWidget *parent = 0);

private slots:
    void experimentStarted();
    void experimentStopped();
    //void scenarioFinished(const QString &error);

private:
    ExperimentContext* _context;
    ScenarioRunner* _runner;
    QList<FrameWidget*> _frames;
    QStringList _images;
    bool _stopFlag = false;

    void makeFrames();
    void prepareImagesBank();
};

#endif // FRAMESPANEL_H
