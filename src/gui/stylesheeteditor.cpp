#include "appconfig.h"
#include "stylesheeteditor.h"
#include "utils.h"
#include "../ori/OriWidgets.h"

#include <QApplication>
#include <QBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>

StyleSheetEditor::StyleSheetEditor(QWidget *parent) : QWidget(parent)
{
    _editor = new QPlainTextEdit;
    _editor->setPlainText(qApp->styleSheet());
    auto f = _editor->font();
    f.setFamily("Courier New");
    f.setPointSize(12);
    _editor->setFont(f);

    auto buttonApply = new QPushButton("Apply");
    connect(buttonApply, SIGNAL(pressed()), this, SLOT(applyStyleSheet()));

    auto buttonSave = new QPushButton("Save");
    connect(buttonSave, SIGNAL(pressed()), this, SLOT(saveStyleSheet()));

    setLayout(Ori::Gui::layoutV(
    {
        _editor,
        Ori::Gui::layoutH({buttonSave, 0, buttonApply})
    }));

    resize(600, 800);
}

void StyleSheetEditor::applyStyleSheet()
{
    qApp->setStyleSheet(_editor->toPlainText());
}

void StyleSheetEditor::saveStyleSheet()
{
    Utils::saveTextToFile(AppConfig::styleSheetFileName(), _editor->toPlainText().toUtf8());
}
