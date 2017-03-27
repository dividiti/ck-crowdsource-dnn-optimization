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
#include "OriWidgets.h"

#include <QBoxLayout>
#include <QLabel>
#include <QObject>
#include <QTreeWidget>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    #define qintptr std::intptr_t
#endif

namespace Ori {
namespace Gui {

//--------------------------------------------------------------------------------------------------

QLabel* makeTitle(const QString& title)
{
    auto label = new QLabel(title);
    label->setProperty("qss-role", "panel-title");
    auto f = label->font();
    f.setLetterSpacing(QFont::AbsoluteSpacing, 0.8);
    label->setFont(f);
    return label;
}

QWidget* makeDivider()
{
    auto divider = new QFrame;
    divider->setProperty("qss-role", "divider");
    divider->setFrameShape(QFrame::HLine);
    return divider;
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

} // namespace Gui
} // namespace Ori


