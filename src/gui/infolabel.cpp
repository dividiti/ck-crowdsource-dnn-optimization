#include "infolabel.h"

#include <QBoxLayout>
#include <QLabel>

InfoLabel::InfoLabel(QWidget *parent) : QVBoxLayout(parent)
{
    _title = new QLabel;
    _title->setObjectName("infoLabelTitle");
    auto f = _title->font();
    f.setLetterSpacing(QFont::AbsoluteSpacing, 0.8);
//    f.setBold(true);
    _title->setFont(f);

    _info = new QLabel;
    _info->setObjectName("infoLabelValue");
//    f = _info->font();
//    f.setPointSize(f.pointSize()+1);
//    _info->setFont(f);

    setMargin(0);
    addWidget(_title);
    addWidget(_info);
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
