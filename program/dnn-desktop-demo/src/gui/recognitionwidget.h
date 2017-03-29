#ifndef RECOGNITIONFRAME
#define RECOGNITIONFRAME

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ImageResult;

class RecognitionWidget : public QWidget {
    Q_OBJECT

public:
    explicit RecognitionWidget(QWidget* parent = Q_NULLPTR);

    void load(const ImageResult& imageResult);
    void clear();

private:

    QLabel* imageLabel;
    QLabel* descriptionLabel;
};

#endif // RECOGNITIONFRAME

