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
#include <QPushButton>

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

FooterPanel::FooterPanel(ExperimentContext* context, QWidget* parent) : QFrame(parent), _context(context) {
    connect(_context, &ExperimentContext::publishStarted, this, &FooterPanel::publishStarted);
    connect(_context, &ExperimentContext::publishFinished, this, &FooterPanel::publishFinished);
    connect(_context, &ExperimentContext::experimentStarted, this, &FooterPanel::enablePublish);
    connect(_context, &ExperimentContext::experimentFinished, this, &FooterPanel::enablePublish);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setFixedHeight(50);
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

    _buttonPublish = new QPushButton(tr("Publish"));
    _buttonPublish->setObjectName("buttonPublish");
    enablePublish();
    connect(_buttonPublish, SIGNAL(clicked(bool)), _context, SLOT(publishResults()));
    layout->addWidget(_buttonPublish);

    setLayout(layout);
}

void FooterPanel::publishStarted() {
    _buttonPublish->setText(tr("Publishing..."));
    _buttonPublish->setEnabled(false);
}

void FooterPanel::publishFinished(bool) {
    _buttonPublish->setText(tr("Publish"));
    enablePublish();
}

void FooterPanel::enablePublish() {
    _buttonPublish->setEnabled(!_context->isExperimentStarted() && _context->hasAggregatedResults());
}
