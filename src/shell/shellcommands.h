#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QApplication;
class QTextStream;
QT_END_NAMESPACE

class ShellCommands : public QObject
{
    Q_OBJECT

public:
    explicit ShellCommands(QObject *parent = 0);

    bool process(const QApplication& app);

private:
    void command_caffeModels();

private:
    QTextStream& cout();
};

#endif // SHELLCOMMANDS_H
