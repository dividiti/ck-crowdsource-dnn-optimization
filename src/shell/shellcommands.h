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
    enum Result { CommandIgnored, CommandFinished, ParamsAcquired };

    explicit ShellCommands(QObject *parent = 0);

    Result process(const QApplication& app);

    AppRunParams appRunParams() const { return _appParams; }

private:
    void command_caffeModels();
    void command_caffeLibs();

private:
    AppRunParams _appParams;

    QTextStream& cout();
};

#endif // SHELLCOMMANDS_H
