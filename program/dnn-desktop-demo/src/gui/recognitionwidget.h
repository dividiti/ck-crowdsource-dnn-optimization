#ifndef RECOGNITIONFRAME
#define RECOGNITIONFRAME

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QScrollArea;
class QFrame;
class QPixmap;
QT_END_NAMESPACE

class ImageResult;
class ExperimentContext;

class RecognitionWidget : public QWidget {
    Q_OBJECT

public:
    explicit RecognitionWidget(ExperimentContext* context, QWidget* parent = Q_NULLPTR);
    ~RecognitionWidget();

    void load(const ImageResult& imageResult);
    void clear();

private slots:

    void rescale(double);

private:

    QLabel* imageLabel;
    QLabel* descriptionLabel;
    ExperimentContext* context;
    QScrollArea* scroll;
    QFrame* imageFrame;
    QPixmap origPixmap;

    void updateScrollArea();
    QPixmap polishPixmap(const QPixmap& pixmap);
};

#endif // RECOGNITIONFRAME

