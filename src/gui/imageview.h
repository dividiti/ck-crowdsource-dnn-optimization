#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QFrame>
#include <QImage>

class ImageView : public QFrame
{
    Q_OBJECT
public:
    ImageView(int w, int h, QWidget* parent = 0);

    void loadImage(const QString& file);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage _image;
};

#endif // IMAGEVIEW_H
