#ifndef ORI_LABELS_H
#define ORI_LABELS_H

#include <QLabel>

namespace Ori {
namespace Widgets {

////////////////////////////////////////////////////////////////////////////////
//                                  Label
////////////////////////////////////////////////////////////////////////////////

class Label : public QLabel
{
public:
    explicit Label(QWidget *parent = 0, Qt::WindowFlags f = 0);
    explicit Label(const QString &text, QWidget *parent = 0, Qt::WindowFlags f = 0);
    void setSizeHint(QSize size) { _sizeHint = size; }
    void setSizeHint(int w, int h) { _sizeHint = QSize(w, h); }
    void setHorizontalSizeHint(int w) { _sizeHint = QSize(w, _sizeHint.height()); }
    void setVerticalSizeHint(int h) { _sizeHint = QSize(_sizeHint.width(), h); }
    void setHorizontalSizePolicy(QSizePolicy::Policy policy) { setSizePolicy(policy, sizePolicy().verticalPolicy()); }
    void setVerticalSizePolicy(QSizePolicy::Policy policy) { setSizePolicy(sizePolicy().horizontalPolicy(), policy); }
    QSize sizeHint() const { return _sizeHint; }
private:
    QSize _sizeHint;
};


////////////////////////////////////////////////////////////////////////////////
//                             ClickableLabel
////////////////////////////////////////////////////////////////////////////////

class ClickableLabel : public Label
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = 0, Qt::WindowFlags f = 0);
    explicit ClickableLabel(const QString &text, QWidget *parent = 0, Qt::WindowFlags f = 0);
signals:
    void clicked();
    void doubleClicked();
protected:
    void mouseReleaseEvent(class QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent*) override { emit doubleClicked(); }
};

////////////////////////////////////////////////////////////////////////////////
//                             LabelSeparator
////////////////////////////////////////////////////////////////////////////////

class LabelSeparator : public QLabel
{
public:
    explicit LabelSeparator(QWidget *parent = 0, Qt::WindowFlags f = 0);
    explicit LabelSeparator(const QString &text, QWidget *parent = 0, Qt::WindowFlags f = 0);
    explicit LabelSeparator(const QString &text, bool bold);
protected:
    void paintEvent(QPaintEvent*);
};

////////////////////////////////////////////////////////////////////////////////
//                               ImagedLabel
////////////////////////////////////////////////////////////////////////////////

class ImagedLabel : public QWidget
{
public:
    explicit ImagedLabel(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void setContent(const QString& text, const QString& path = QString());
    QLabel *iconLabel() const { return _icon; }
    QLabel *textLabel() const { return _text; }
    void clear() { _icon->clear(); _text->clear(); }
private:
    QLabel *_icon, *_text;
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_LABELS_H
