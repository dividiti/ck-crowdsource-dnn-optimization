#include "engineselectordialog.h"
#include "appconfig.h"
#include "appevents.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDesktopWidget>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QScreen>
#include <QScrollArea>
#include <QSpinBox>
#include <QStyle>
#include <QTextBrowser>

namespace {
QScreen* findScreenOrPrimary(QWidget* w)
{
    if (!w)
        return QGuiApplication::primaryScreen();
    int screenNumber = qApp->desktop()->screenNumber(w);
    if (screenNumber < 0)
        return QGuiApplication::primaryScreen();
    return QGuiApplication::screens().at(screenNumber);
}
}

EngineSelectorDialog::EngineSelectorDialog(QWidget *parent) : QDialog(parent)
{

}

QVariant EngineSelectorDialog::selectEngineAndBatchSize()
{
    QVariant ret;

    const Mode::Type m = AppConfig::currentModeType();

    auto items = AppConfig::programs(m);
    if (items.isEmpty()) {
        AppEvents::info("Recognition engines not found");
        return ret;
    }

    EngineSelectorDialog dlg;

    auto current = AppConfig::currentProgram(m);
    auto layout = new QVBoxLayout;

    auto flagsWidget = new QWidget;
    auto flagsLayout = new QGridLayout(flagsWidget);
    enum FlagsColumn { FLAG_COL_NAME, FLAG_COL_VERSION, FLAG_COL_SPACING, FLAG_COL_INFO };

    QFontMetrics basicFontMetrics(dlg.font());
    int maxTitleW = 0, maxVersionW = 0;
    int depsLabelW = basicFontMetrics.boundingRect("Deps").width();

    QFont versionFont = dlg.font();
    versionFont.setBold(true);
    QFontMetrics versionFontMetrics(versionFont);

    QVector<QRadioButton*> flags;
    for (int i = 0; i < items.size(); i++) {
        auto title = items.at(i).title();
        auto flag = new QRadioButton(title);
        flag->setChecked(current.isValid() && items[i] == current.value<Program>());
        flagsLayout->addWidget(flag, i, FLAG_COL_NAME);
        flags << flag;
        int titleW = basicFontMetrics.boundingRect(title).width();

        auto version = items.at(i).targetVersion;
        int versionW = 0;
        if (!version.isEmpty()) {
            auto versionLabel = new QLabel(version);
            versionLabel->setFont(versionFont);
            flagsLayout->addWidget(versionLabel, i, FLAG_COL_VERSION);
            versionW += versionFontMetrics.boundingRect(versionLabel->text()).width();
        }

        auto targetDepsInfo = items.at(i).targetDepsInfo;
        if (!targetDepsInfo.isEmpty()) {
            auto infoKey = QString::number(i);
            auto infoLabel = new QLabel(QString("<a href='%1'>Deps</a>").arg(infoKey));
            infoLabel->setProperty("qss-role", "link");
            connect(infoLabel, SIGNAL(linkActivated(QString)), &dlg, SLOT(showInfo(QString)));
            flagsLayout->addWidget(infoLabel, i, FLAG_COL_INFO);
            dlg.flagsInfo.insert(infoKey, targetDepsInfo);
            titleW += depsLabelW;
        }

        maxTitleW = qMax(maxTitleW, titleW);
        maxVersionW = qMax(maxVersionW, versionW);
    }

    auto flagsScrollArea = new QScrollArea;
    flagsScrollArea->setWidget(flagsWidget);
    layout->addWidget(flagsScrollArea);

    QSpinBox *batchSize = nullptr;

    if (Mode::Type::CLASSIFICATION == m) {
        batchSize = new QSpinBox;
        batchSize->setMinimum(1);
        batchSize->setSingleStep(1);
        batchSize->setValue(AppConfig::batchSize());

        auto batchSizeWidget = new QWidget;
        auto batchSizeLayout = new QHBoxLayout;
        batchSizeLayout->addWidget(new QLabel("Batch size: "));
        batchSizeLayout->addWidget(batchSize);
        batchSizeWidget->setLayout(batchSizeLayout);

        layout->addWidget(batchSizeWidget);
    }

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    dlg.connect(buttons, SIGNAL(accepted()), &dlg, SLOT(accept()));
    dlg.connect(buttons, SIGNAL(rejected()), &dlg, SLOT(reject()));
    layout->addSpacing(6);
    layout->addWidget(buttons);

    dlg.setLayout(layout);
    dlg.adjustSize();

    // Roughgly calculate size of dialog basing on the text size and giving some ad-hoc margins
    const int extraW = 80;
    int dlgW = dlg.width();
    if (dlgW < maxTitleW + maxVersionW + extraW) {
        auto screenRect = findScreenOrPrimary(nullptr)->availableGeometry();
        dlgW = qMin(maxTitleW + maxVersionW + extraW, screenRect.width() * 3/4);
    }
    // Take extra width to give a room for vertical scroll bar
    if (flagsWidget->height() > flagsScrollArea->height()) {
        dlgW += qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    }
    dlg.resize(dlgW, dlg.height());

    if (dlg.exec() == QDialog::Accepted) {
        if (Mode::Type::CLASSIFICATION == m) {
            AppConfig::setBatchSize(batchSize->value());
        }
        for (int i = 0; i < flags.size(); i++) {
            if (flags.at(i)->isChecked()) {
                ret.setValue(items[i]);
                break;
            }
        }
    }
    return ret;
}

void EngineSelectorDialog::showInfo(const QString& key)
{
    if (!flagsInfo.contains(key)) return;

    QStringList report;
    report << "Dependencies:<ul>";
    for (auto line : flagsInfo[key].split(';')) {
        auto keyValue = line.split(' ');
        if (keyValue.length() == 2) {
            report << QString("<li>%1 <b>%2</b>").arg(keyValue[0], keyValue[1]);
        } else {
            report << line;
        }
    }
    report << "</ul>";

    QDialog dlg;

    auto layout = new QVBoxLayout(&dlg);
    layout->setSpacing(6);

    auto textView = new QTextBrowser;
    textView->setHtml(report.join(""));
    layout->addWidget(textView);

    // Roughly calculate size of text
    int textH = 0, textW = 0;
    QFontMetrics fontMetrics(textView->font());
    for (const QString& line: report) {
        auto r = fontMetrics.boundingRect(line);
        textH += r.height();
        textW = qMax(textW, r.width());
    }

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    dlg.connect(buttons, SIGNAL(accepted()), &dlg, SLOT(accept()));
    layout->addWidget(buttons);

    // Roughgly calculate size of dialog basing on the text size and giving some ad-hoc margins
    auto screenRect = findScreenOrPrimary(this)->availableGeometry();
    int dlgW = qMin(textW + 100, screenRect.width() * 3/4);
    int dlgH = qMin(textH + 100, screenRect.height() * 3/4);

    dlg.resize(dlgW, dlgH);
    dlg.exec();
}
