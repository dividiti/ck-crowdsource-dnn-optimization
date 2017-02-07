#ifndef RESULTSPANEL_H
#define RESULTSPANEL_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ExperimentContext;
class ImageView;
class InfoLabel;

class ResultsPanel : public QFrame
{
    Q_OBJECT
public:
    explicit ResultsPanel(ExperimentContext* context, QWidget *parent = 0);

private slots:
    void experimentStarted();
    void experimentResultReady();

private:
    ExperimentContext* _context;
    QLabel *_infoTimePerImage, *_infoImagesPerSec;
    ImageView *_worstPredictedImage;
};

#endif // RESULTSPANEL_H
