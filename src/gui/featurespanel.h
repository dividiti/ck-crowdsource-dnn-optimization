#ifndef FEATURESPANEL_H
#define FEATURESPANEL_H

#include <QFrame>
#include <QPointer>

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
    void setBatchSize();
    void experimentStarted();
    void experimentFinished();

private:
    ExperimentContext* _context;
    InfoLabel *_infoEngine, *_infoModel, *_infoImages, *_infoBatchSize;
    QWidget *_linkSelectEngine, *_linkSelectModel, *_linkSelectImages, *_linkSetBatchSize;

    QWidget* makeLink(const QString &text, const QString& tooltip, const char* slot);
    void enableControls(bool on);
};

#endif // FEATURESPANEL_H
