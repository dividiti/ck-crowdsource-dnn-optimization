#include "infolabel.h"

#include <QBoxLayout>
#include <QLabel>

InfoLabel::InfoLabel(QWidget *parent) : QWidget(parent)
{
    _title = new QLabel;
    auto f = _title->font();
    f.setBold(true);
    _title->setFont(f);

    _info = new QLabel;
    f = _info->font();
    f.setPointSize(f.pointSize()+1);
    _info->setFont(f);

    setLayout(new QVBoxLayout);
    layout()->setMargin(0);
    layout()->addWidget(_title);
    layout()->addWidget(_info);
}

InfoLabel::InfoLabel(const QString& title, QWidget *parent): InfoLabel(parent)
{
    setTitle(title);
}

void InfoLabel::setTitle(const QString& value)
{
    _title->setText(value);
}

void InfoLabel::setInfo(const QString& value)
{
    _info->setText(value);
}
