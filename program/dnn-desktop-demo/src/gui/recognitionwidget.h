#ifndef RECOGNITIONFRAME
#define RECOGNITIONFRAME

#include "appmodels.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QScrollArea;
class QFrame;
class QPixmap;
QT_END_NAMESPACE

class ExperimentContext;

class RecognitionWidget : public QWidget {
    Q_OBJECT

public:
    explicit RecognitionWidget(ExperimentContext* context, QWidget* parent = Q_NULLPTR);

    void load(const ImageResult& imageResult);

private slots:

    void rescale(double, bool);

private:

    QLabel* descriptionLabel;
    ExperimentContext* context;
    QScrollArea* scroll;
    QPixmap origPixmap;
    ImageResult imageResult;

    void updateScrollArea();
    QPixmap preparePixmap();
};

#endif // RECOGNITIONFRAME

