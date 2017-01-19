#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

class ShellCommands
{
public:
    static bool process(const class QApplication& app);

private:
    static void command_querySharedResourcesInfo();
};

#endif // SHELLCOMMANDS_H
