#ifndef STYLESHEETEDITOR_H
#define STYLESHEETEDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

class StyleSheetEditor : public QWidget
{
    Q_OBJECT
public:
    explicit StyleSheetEditor(QWidget *parent = 0);

private slots:
    void applyStyleSheet();
    void saveStyleSheet();

private:
    QPlainTextEdit* _editor;
};

#endif // STYLESHEETEDITOR_H
