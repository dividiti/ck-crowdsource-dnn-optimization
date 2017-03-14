/*
 * MIT License
 *
 * Copyright (c) 2017 Nikolay Chunosov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ORI_WIDGETS_H
#define ORI_WIDGETS_H

#include <QString>
#include <QKeySequence>

QT_BEGIN_NAMESPACE
class QAction;
class QBoxLayout;
class QComboBox;
class QGroupBox;
class QLabel;
class QMenu;
class QObject;
class QPushButton;
class QSplitter;
class QTableView;
class QTextBrowser;
class QToolBar;
class QToolButton;
class QTreeWidget;
class QWidget;
QT_END_NAMESPACE

namespace Ori {
namespace Gui {

QLabel* makeTitle(const QString &title);
QWidget* makeDivider();
void adjustFont(QWidget*);
void setFontSizePt(QWidget *w, int sizePt);
void setFontMonospace(QWidget *w, int sizePt = 0);

QSplitter* splitterH(QWidget*, QWidget*);
QSplitter* splitterV(QWidget*, QWidget*);
QSplitter* splitterH(QWidget*, QWidget*, int size1, int size2);
QSplitter* splitterV(QWidget*, QWidget*, int size1, int size2);

QToolButton* textToolButton(QAction* action);
QToolButton* menuToolButton(QMenu* menu, QAction* action = nullptr);
QToolButton* iconToolButton(const QString& tooltip, const QString& iconPath, int iconSize, QObject* receiver, const char* slot);

QMenu* menu(std::initializer_list<QObject*> items);
QMenu* menu(QWidget *parent, std::initializer_list<QObject*> items);
QMenu* menu(const QString& title, std::initializer_list<QObject*> items);
QMenu* menu(const QString& title, QWidget *parent, std::initializer_list<QObject*> items);
QMenu* populate(QMenu* menu, std::initializer_list<QObject*> items);
void append(QMenu* menu, QObject* item);

QToolBar* toolbar(std::initializer_list<QObject*> items);
QToolBar* populate(QToolBar* toolbar, std::initializer_list<QObject*> items);
void append(QToolBar* toolbar, QObject* item);

QBoxLayout* layoutH(const std::initializer_list<QObject*>& items);
QBoxLayout* layoutV(const std::initializer_list<QObject*>& items);
QBoxLayout* layoutH(int margin, int spacing, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutV(int margin, int spacing, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutH(QWidget* parent, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutV(QWidget* parent, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutH(QWidget* parent, int margin, int spacing, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutV(QWidget* parent, int margin, int spacing, const std::initializer_list<QObject*>& items);
QBoxLayout* populate(QBoxLayout* layout, const std::initializer_list<QObject*>& items);
void append(QBoxLayout* layout, QObject* item);
QObject* spacing(int size);
QObject* defaultSpacing(int factor = 1);

QLabel* stretchedLabel(const QString& text);

int layoutSpacing();
int borderWidth();

QGroupBox* group(const QString& title, QBoxLayout* layout);
QGroupBox* groupV(const QString& title, const std::initializer_list<QObject*>& items);
QGroupBox* groupH(const QString& title, const std::initializer_list<QObject*>& items);

QWidget* widget(QBoxLayout* layout);
QWidget* widgetV(const std::initializer_list<QObject*>& items);
QWidget* widgetH(const std::initializer_list<QObject*>& items);

QPushButton* button(const QString& title, QObject* receiver, const char* slot);

void setSelectedId(QComboBox *combo, int id);
int getSelectedId(const QComboBox *combo, int def = -1);

QAction* action(const QString& title, QObject* receiver, const char* slot, const char* icon = 0, const QKeySequence& shortcut = QKeySequence());
QAction* toggledAction(const QString& title, QObject* receiver, const char* slot, const char* icon = 0, const QKeySequence& shortcut = QKeySequence());

QTextBrowser* logView(int sizePt = 0);

QTreeWidget* twoColumnTree(const QString& title1, const QString& title2);

void stretchColumn(QTableView *table, int col);
void resizeColumnToContent(QTableView *table, int col);
} // namespace Gui
} // namespace Ori

#endif // ORI_WIDGETS_H
