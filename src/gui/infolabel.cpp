#include "infolabel.h"

#include <QBoxLayout>
#include <QLabel>

InfoLabel::InfoLabel(QWidget *parent) : QVBoxLayout(parent)
{
    _title = new QLabel;
    _title->setObjectName("infoLabelTitle");
    auto f = _title->font();
    f.setLetterSpacing(QFont::AbsoluteSpacing, 0.8);
    _title->setFont(f);

    _info = new QLabel;
    _info->setObjectName("infoLabelValue");

    _infoAux = new QLabel;
    _infoAux->setObjectName("infoLabelValueAux");
    _infoAux->setVisible(false);

    setMargin(0);
    setSpacing(0);
    addWidget(_title);
    addWidget(_info);
    addWidget(_infoAux);
}

InfoLabel::InfoLabel(const QString& title, QWidget *parent): InfoLabel(parent)
{
    setTitle(title);
}

void InfoLabel::setTitle(const QString& value)
{
    _title->setText(value);
}

void InfoLabel::setInfo(const QString& value, const QString &auxValue)
{
    _info->setText(value);
    _infoAux->setText(auxValue);
    _infoAux->setVisible(!auxValue.isEmpty());
}
