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
class QBoxLayout;
class QLabel;
class QObject;
class QWidget;
QT_END_NAMESPACE

namespace Ori {
namespace Gui {

QLabel* makeTitle(const QString &title);
QWidget* makeDivider();

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

} // namespace Gui
} // namespace Ori

#endif // ORI_WIDGETS_H
