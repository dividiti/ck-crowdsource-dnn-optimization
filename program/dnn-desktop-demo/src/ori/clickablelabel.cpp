#include "clickablelabel.h"

#include <QLabel>
#include <QDesktopServices>

ClickableLabel::ClickableLabel(const QUrl& url) : QLabel(0), url(url) {
    setCursor(Qt::PointingHandCursor);
}

void ClickableLabel::mousePressEvent(QMouseEvent*) {
    QDesktopServices::openUrl(url);
}
