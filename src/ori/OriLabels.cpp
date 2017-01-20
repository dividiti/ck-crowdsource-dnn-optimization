#include "OriLabels.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>

namespace Ori {
namespace Widgets {

////////////////////////////////////////////////////////////////////////////////
//                                  Label
////////////////////////////////////////////////////////////////////////////////

Label::Label(QWidget *parent, Qt::WindowFlags f) : QLabel(parent, f)
{
    _sizeHint = QLabel::sizeHint();
}

Label::Label(const QString &text, QWidget *parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{
    _sizeHint = QLabel::sizeHint();
}

////////////////////////////////////////////////////////////////////////////////
//                             ClickableLabel
////////////////////////////////////////////////////////////////////////////////

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f) :Label(parent, f)
{
}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : Label(text, parent, f)
{
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton)
        emit clicked();
}

////////////////////////////////////////////////////////////////////////////////
//                             LabelSeparator
////////////////////////////////////////////////////////////////////////////////

LabelSeparator::LabelSeparator(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent, f) {}

LabelSeparator::LabelSeparator(const QString &text, QWidget *parent,
                               Qt::WindowFlags f) : QLabel(text, parent, f)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

LabelSeparator::LabelSeparator(const QString &text, bool bold) : QLabel(text)
{
    if (bold)
    {
        QFont f = font();
        f.setBold(true);
        setFont(f);
    }
}

void LabelSeparator::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QRect cr = contentsRect();
    int m = margin();
    cr.adjust(m, m, -m, -m);
    QString text = QLabel::text();
    QPalette palette = QWidget::palette();
    if (!text.isEmpty())
    {
        QStyle *style = QWidget::style();
        int align = QStyle::visualAlignment(layoutDirection(), alignment());
        style->drawItemText(&p, cr, align, palette, true, text, foregroundRole());
        p.setClipRegion(QRegion(cr) - style->itemTextRect(
                            QFontMetrics(font()), cr, align, true, text).adjusted(-4, 0, 4, 0));
    }
    int y = (cr.bottom() + cr.top()) / 2;
    p.setPen(palette.color(QPalette::Mid));
    p.drawLine(cr.left(), y, cr.right(), y);
    p.setPen(palette.color(QPalette::Midlight));
    p.drawLine(cr.left(), y+1, cr.right(), y+1);
}

////////////////////////////////////////////////////////////////////////////////
//                              ImagedLabel
////////////////////////////////////////////////////////////////////////////////

ImagedLabel::ImagedLabel(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    _icon = new QLabel;
    _text = new QLabel;

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(2);
    layout->setSpacing(3);
    layout->addWidget(_icon);
    layout->addWidget(_text);
    setLayout(layout);
}

void ImagedLabel::setContent(const QString& text, const QString& path)
{
    _text->setText(text);
    if (path.isEmpty())
        _icon->clear();
    else _icon->setPixmap(QIcon(path).pixmap(16, 16));
}

} // namespace Widgets
} // namespace Ori
