#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QUrl>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(const QUrl& url);

protected:
    virtual void mousePressEvent(QMouseEvent*) override;

private:
    const QUrl url;
};

#endif // CLICKABLELABEL_H
