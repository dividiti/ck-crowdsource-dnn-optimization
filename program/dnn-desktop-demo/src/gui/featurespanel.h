#ifndef FEATURESPANEL_H
#define FEATURESPANEL_H

#include <QFrame>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ExperimentContext;
class InfoLabel;

class FeaturesPanel : public QFrame
{
    Q_OBJECT
public:
    explicit FeaturesPanel(ExperimentContext *context, QWidget *parent = 0);

    void updateExperimentConditions();

private slots:
    void selectEngine();
    void selectModel();
    void selectImages();
    void selectMode();
    void experimentStarted();
    void experimentFinished();

private:
    ExperimentContext* _context;
    QLabel *_infoEngine, *_infoModel, *_infoImages, *_infoMode;
    QWidget *_linkSelectEngine, *_linkSelectModel, *_linkSelectImages, *_linkSelectMode;

    QWidget* makeLink(const QString &text, const QString& tooltip, const char* slot);
    QLabel* makeInfoLabel();
    void enableControls(bool on);
};

#endif // FEATURESPANEL_H
