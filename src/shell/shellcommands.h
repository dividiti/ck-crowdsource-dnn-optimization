#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include <QObject>

#include "appmodels.h"
#include "remotedataaccess.h"

QT_BEGIN_NAMESPACE
class QApplication;
class QTextStream;
QT_END_NAMESPACE

class ScenariosProvider;

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

    RemoteDataAccess _network;
    ScenariosProvider *_scenariosProvider = nullptr;
    bool _saveLoadedScenariosToCache = false;
};

#endif // SHELLCOMMANDS_H
