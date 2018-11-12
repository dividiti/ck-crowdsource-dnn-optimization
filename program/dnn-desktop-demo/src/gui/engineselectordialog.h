#ifndef ENGINESELECTORDIALOG_H
#define ENGINESELECTORDIALOG_H

#include <QDialog>
#include <QMap>

class EngineSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    static QVariant selectEngineAndBatchSize();

private slots:
    void showInfo(const QString& key);

private:
    QMap<QString, QString> flagsInfo;

    explicit EngineSelectorDialog(QWidget *parent = nullptr);
};

#endif // ENGINESELECTORDIALOG_H
