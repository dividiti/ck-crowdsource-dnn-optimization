#include "footerpanel.h"
#include "../ori/clickablelabel.h"
#include "appconfig.h"

#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QVariant>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>

static QLabel* createCredential(const QString& text, const QUrl& url) {
    auto ret = new ClickableLabel(url);
    ret->setText(text);
    ret->setProperty("qss-role", "credential");
    return ret;
}

static QLabel* createLogo(const QString& name, const QUrl& url) {
    auto ret = new ClickableLabel(url);
    ret->setPixmap(QPixmap(":/images/logo-" + name));
    return ret;
}

static void addCompany(QBoxLayout* layout, QString name, QString host) {
    QUrl url("http://" + host, QUrl::TolerantMode);
    layout->addWidget(createLogo(name, url));
    layout->addSpacing(18);
    layout->addWidget(createCredential(host, url));
    layout->addSpacing(40);
}

FooterPanel::FooterPanel(QWidget* parent) : QFrame(parent) {
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setFixedHeight(40);
    setObjectName("footerPanel");

    auto layout = new QHBoxLayout;\
    layout->addSpacing(20);
    addCompany(layout, "dividiti", "dividiti.com");
    addCompany(layout, "xored", "xored.com");
    addCompany(layout, "cknowledge", "cknowledge.org/ai");

    layout->addStretch();

    auto rightText = AppConfig::footerRightText();
    if (!rightText.isEmpty()) {
        auto rightUrl = AppConfig::footerRightUrl();
        auto label = rightUrl.isEmpty() ? new QLabel() : new ClickableLabel(QUrl(rightUrl));
        label->setProperty("qss-role", "credential");
        label->setText(rightText);
        layout->addWidget(label);
    }
    setLayout(layout);
}

