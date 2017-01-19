#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include <QObject>

#include "appmodels.h"

QT_BEGIN_NAMESPACE
class QApplication;
class QTextStream;
QT_END_NAMESPACE

class ShellCommands : public QObject
{
    Q_OBJECT

public:
    explicit ShellCommands(QObject *parent = 0) : QObject(parent) {}

    bool process(const QApplication& app);

private:
    void command_querySharedRepoInfo();
    void command_showCachedPlatformFeatures();
    void command_loadScenariosForCachedFeatures();

private slots:
    void sharedRepoInfoReceived(SharedRepoInfo info);
    void scenariosReceived(RecognitionScenarios scenarios);

private:
    QTextStream& cout();
};

#endif // SHELLCOMMANDS_H
