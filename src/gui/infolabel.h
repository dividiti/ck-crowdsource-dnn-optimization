#ifndef INFOLABEL_H
#define INFOLABEL_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class InfoLabel : public QWidget
{
    Q_OBJECT

public:
    explicit InfoLabel(QWidget *parent = 0);
    explicit InfoLabel(const QString& title, QWidget *parent = 0);

    void setTitle(const QString& value);
    void setInfo(const QString& value);

private:
    QLabel *_title, *_info;
};

#endif // INFOLABEL_H
