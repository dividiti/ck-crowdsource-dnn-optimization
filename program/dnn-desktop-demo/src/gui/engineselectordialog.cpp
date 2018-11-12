#include "engineselectordialog.h"
#include "appconfig.h"
#include "appevents.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStyle>
#include <QTextBrowser>

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

    QVector<QRadioButton*> flags;
    for (int i = 0; i < items.size(); i++) {
        auto flag = new QRadioButton(items.at(i).title());
        flag->setChecked(current.isValid() && items[i] == current.value<Program>());
        flagsLayout->addWidget(flag, i, FLAG_COL_NAME);
        flags << flag;

        auto version = items.at(i).targetVersion;
        if (!version.isEmpty()) {
            auto versionLabel = new QLabel("<b>" + version + "</b>");
            flagsLayout->addWidget(versionLabel, i, FLAG_COL_VERSION);
        }

        auto targetDepsInfo = items.at(i).targetDepsInfo;
        if (!targetDepsInfo.isEmpty()) {
            auto infoKey = QString::number(i);
            auto infoLabel = new QLabel(QString("<a href='%1'>Deps info</a>").arg(infoKey));
            infoLabel->setProperty("qss-role", "link");
            connect(infoLabel, SIGNAL(linkActivated(QString)), &dlg, SLOT(showInfo(QString)));
            flagsLayout->addWidget(infoLabel, i, FLAG_COL_INFO);
            dlg.flagsInfo.insert(infoKey, targetDepsInfo);
        }
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

    // Take extra width to give a room for vertical scroll bar
    if (flagsWidget->height() > flagsScrollArea->height()) {
        int w = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        dlg.resize(dlg.width() + w, dlg.height());
    }

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

    auto textView = new QTextBrowser;
    textView->setHtml(report.join(""));
    layout->addWidget(textView);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    dlg.connect(buttons, SIGNAL(accepted()), &dlg, SLOT(accept()));
    layout->addSpacing(6);
    layout->addWidget(buttons);

    dlg.exec();
}
