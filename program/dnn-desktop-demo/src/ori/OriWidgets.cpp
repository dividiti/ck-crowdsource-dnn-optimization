#include "OriWidgets.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QObject>
#include <QPushButton>
#include <QStyle>
#include <QSplitter>
#include <QTableView>
#include <QTextBrowser>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    #define qintptr std::intptr_t
#endif

namespace Ori {
namespace Gui {

//--------------------------------------------------------------------------------------------------

void adjustFont(QWidget *w)
{
#ifdef Q_OS_WIN
    QFont f = w->font();
    if (f.pointSize() < 10)
    {
        f.setPointSize(10);
        w->setFont(f);
    }
#else
    Q_UNUSED(w)
#endif
}

void setFontSizePt(QWidget *w, int sizePt)
{
    QFont f = w->font();
    f.setPointSize(sizePt);
    w->setFont(f);
}

void setFontMonospace(QWidget *w, int sizePt)
{
    QFont f = w->font();
#ifdef Q_OS_WIN
    f.setPointSize(10);
    f.setFamily("Courier New");
#else
    f.setFamily("monospace");
#endif
    if (sizePt > 0)
        f.setPointSize(10);
    w->setFont(f);
}

//--------------------------------------------------------------------------------------------------

QSplitter* splitter(Qt::Orientation orientation, QWidget *w1, QWidget *w2, QList<int> sizes)
{
    auto splitter = new QSplitter(orientation);
    splitter->addWidget(w1);
    splitter->addWidget(w2);
    if (!sizes.isEmpty())
        splitter->setSizes(sizes);
    return splitter;
}

QSplitter* splitterH(QWidget *w1, QWidget *w2)
{
    return splitter(Qt::Horizontal, w1, w2, {});
}

QSplitter* splitterV(QWidget *w1, QWidget *w2)
{
    return splitter(Qt::Vertical, w1, w2, {});
}

QSplitter* splitterH(QWidget *w1, QWidget *w2, int size1, int size2)
{
    return splitter(Qt::Horizontal, w1, w2, {size1, size2});
}

QSplitter* splitterV(QWidget *w1, QWidget *w2, int size1, int size2)
{
    return splitter(Qt::Vertical, w1, w2, {size1, size2});
}

//--------------------------------------------------------------------------------------------------

QToolButton* textToolButton(QAction* action)
{
    auto button = new QToolButton;
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    return button;
}

QToolButton* menuToolButton(QMenu* menu, QAction* action)
{
    auto button = new QToolButton;
    button->setDefaultAction(action);
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setMenu(menu);
    return button;
}

QToolButton* iconToolButton(const QString& tooltip, const QString& iconPath, int iconSize, QObject* receiver, const char* slot)
{
    auto button = new QToolButton;
    button->setToolTip(tooltip);
    button->setIconSize(QSize(iconSize, iconSize));
    button->setIcon(QIcon(iconPath));
    button->connect(button, SIGNAL(clicked()), receiver, slot);
    return button;
}

//--------------------------------------------------------------------------------------------------

QMenu* menu(std::initializer_list<QObject*> items)
{
    return menu(QString(), nullptr, items);
}

QMenu* menu(QWidget *parent, std::initializer_list<QObject*> items)
{
    return menu(QString(), parent, items);
}

QMenu* menu(const QString& title, std::initializer_list<QObject*> items)
{
    return menu(title, nullptr, items);
}

QMenu* menu(const QString& title, QWidget *parent, std::initializer_list<QObject*> items)
{
    return populate(new QMenu(title, parent), items);
}

QMenu* populate(QMenu* menu, std::initializer_list<QObject*> items)
{
    if (!menu) return nullptr;

    menu->clear();
    for (auto item: items)
        append(menu, item);

    return menu;
}

void append(QMenu* menu, QObject* item)
{
    if (!item)
    {
        menu->addSeparator();
        return;
    }
    auto action = qobject_cast<QAction*>(item);
    if (action)
    {
        menu->addAction(action);
        return;
    }
    auto submenu = qobject_cast<QMenu*>(item);
    if (submenu)
    {
        menu->addMenu(submenu);
        return;
    }
}

//--------------------------------------------------------------------------------------------------

QToolBar* toolbar(std::initializer_list<QObject*> items)
{
    return populate(new QToolBar, items);
}

QToolBar* populate(QToolBar* toolbar, std::initializer_list<QObject*> items)
{
    if (!toolbar) return nullptr;

    toolbar->clear();
    for (auto item : items)
        append(toolbar, item);

    return toolbar;
}

void append(QToolBar* toolbar, QObject* item)
{
    if (!item)
    {
        toolbar->addSeparator();
        return;
    }
    auto action = qobject_cast<QAction*>(item);
    if (action)
    {
        toolbar->addAction(action);
        return;
    }
    auto widget = qobject_cast<QWidget*>(item);
    if (widget)
    {
        toolbar->addWidget(widget);
        return;
    }
}

//--------------------------------------------------------------------------------------------------

QBoxLayout* layoutH(const std::initializer_list<QObject*>& items)
{
    return populate(new QHBoxLayout, items);
}

QBoxLayout* layoutV(const std::initializer_list<QObject*>& items)
{
    return populate(new QVBoxLayout, items);
}

QBoxLayout* layoutH(QWidget* parent, const std::initializer_list<QObject*>& items)
{
    return populate(new QHBoxLayout(parent), items);
}

QBoxLayout* layoutV(QWidget* parent, const std::initializer_list<QObject*>& items)
{
    return populate(new QVBoxLayout(parent), items);
}

QBoxLayout* initGeometry(QBoxLayout* layout, int margin, int spacing)
{
    if (margin >= 0) layout->setMargin(margin);
    if (spacing >= 0) layout->setSpacing(spacing);
    return layout;
}

QBoxLayout* layoutH(QWidget* parent, int margin, int spacing, const std::initializer_list<QObject*>& items)
{
    return initGeometry(layoutH(parent, items), margin, spacing);
}

QBoxLayout* layoutV(QWidget* parent, int margin, int spacing, const std::initializer_list<QObject*>& items)
{
    return initGeometry(layoutV(parent, items), margin, spacing);
}

QBoxLayout* layoutH(int margin, int spacing, const std::initializer_list<QObject*>& items)
{
    return initGeometry(layoutH(items), margin, spacing);
}

QBoxLayout* layoutV(int margin, int spacing, const std::initializer_list<QObject*>& items)
{
    return initGeometry(layoutV(items), margin, spacing);
}

QBoxLayout* populate(QBoxLayout* layout, const std::initializer_list<QObject*>& items)
{
    if (!layout) return nullptr;

    for (auto item: items)
        append(layout, item);

    return layout;
}

void append(QBoxLayout* layout, QObject* item)
{
    if (!item)
    {
        layout->addStretch();
        return;
    }
    if (qintptr(item) < 100)
    {
        layout->addSpacing(int(qintptr(item)));
        return;
    }
    auto widget = qobject_cast<QWidget*>(item);
    if (widget)
    {
        layout->addWidget(widget);
        return;
    }
    auto sublayout = qobject_cast<QLayout*>(item);
    if (sublayout)
    {
        layout->addLayout(sublayout);
        return;
    }
}

QObject* spacing(int size) { return (QObject*)qintptr(size); }
QObject* defaultSpacing(int factor) { return spacing(factor * layoutSpacing()); }

//--------------------------------------------------------------------------------------------------

QLabel* stretchedLabel(const QString& text)
{
    auto label = new QLabel(text);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    return label;
}

//--------------------------------------------------------------------------------------------------

int layoutSpacing() { return qApp->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing); }
int borderWidth() { return qApp->style()->pixelMetric(QStyle::PM_DefaultFrameWidth); }

//--------------------------------------------------------------------------------------------------

QGroupBox* group(const QString& title, QBoxLayout* layout)
{
    auto group = new QGroupBox(title);
    group->setLayout(layout);
    return group;
}

QGroupBox* groupV(const QString& title, const std::initializer_list<QObject*>& items)
{
    return group(title, layoutV(items));
}

QGroupBox* groupH(const QString& title, const std::initializer_list<QObject*>& items)
{
    return group(title, layoutH(items));
}

//--------------------------------------------------------------------------------------------------

QWidget* widget(QBoxLayout* layout)
{
    auto widget = new QWidget;
    widget->setLayout(layout);
    return widget;
}

QWidget* widgetV(const std::initializer_list<QObject*>& items)
{
    return widget(layoutV(items));
}

QWidget* widgetH(const std::initializer_list<QObject*>& items)
{
    return widget(layoutH(items));
}

//--------------------------------------------------------------------------------------------------

QPushButton* button(const QString& title, QObject *receiver, const char* slot)
{
    auto button = new QPushButton(title);
    button->connect(button, SIGNAL(clicked(bool)), receiver, slot);
    return button;
}

//--------------------------------------------------------------------------------------------------

void setSelectedId(QComboBox *combo, int id)
{
    for (int i = 0; i < combo->count(); i++)
        if (combo->itemData(i).toInt() == id)
        {
            combo->setCurrentIndex(i);
            return;
        }
    combo->setCurrentIndex(-1);
}

int getSelectedId(const QComboBox *combo, int def)
{
    QVariant data = combo->itemData(combo->currentIndex());
    if (!data.isValid()) return def;
    bool ok;
    int id = data.toInt(&ok);
    return ok? id: def;
}

//--------------------------------------------------------------------------------------------------

QAction* action(const QString& title, QObject* receiver, const char* slot, const char* icon, const QKeySequence& shortcut)
{
    auto action = new QAction(title, receiver);
    action->setShortcut(shortcut);
    if (icon) action->setIcon(QPixmap(icon));
    qApp->connect(action, SIGNAL(triggered()), receiver, slot);
    return action;
}

QAction* toggledAction(const QString& title, QObject* receiver, const char* slot, const char* icon, const QKeySequence& shortcut)
{
    auto action = new QAction(title, receiver);
    action->setShortcut(shortcut);
    action->setCheckable(true);
    if (icon) action->setIcon(QPixmap(icon));
    if (slot) qApp->connect(action, SIGNAL(toggled(bool)), receiver, slot);
    return action;
}

//--------------------------------------------------------------------------------------------------

QTextBrowser* logView(int sizePt)
{
    auto log = new QTextBrowser;
    setFontMonospace(log, sizePt);
    return log;
}

//--------------------------------------------------------------------------------------------------

QTreeWidget* twoColumnTree(const QString& title1, const QString& title2)
{
    auto tree = new QTreeWidget;
    tree->setColumnCount(2);
    tree->setAlternatingRowColors(true);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
#else
    tree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    tree->header()->setResizeMode(1, QHeaderView::Stretch);
#endif

    auto header = new QTreeWidgetItem();
    header->setText(0, title1);
    header->setText(1, title2);
    tree->setHeaderItem(header);

    return tree;
}

//--------------------------------------------------------------------------------------------------

void stretchColumn(QTableView *table, int col)
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    #else
        table->horizontalHeader()->setResizeMode(col, QHeaderView::Stretch);
    #endif
}

void resizeColumnToContent(QTableView *table, int col)
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    #else
        table->horizontalHeader()->setResizeMode(col, QHeaderView::ResizeToContents);
    #endif
}


} // namespace Gui
} // namespace Ori


