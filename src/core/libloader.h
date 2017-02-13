#ifndef LOBLOADED_H
#define LOBLOADED_H

#include <QString>
#include <QList>

class LibLoader
{
public:
    virtual ~LibLoader() {}
    static LibLoader* create();

    virtual void loadDeps(const QStringList &deps) = 0;
    virtual void loadLib(const QString& lib) = 0;
    virtual void* resolve(const char* symbol) = 0;

protected:
    LibLoader() {}
};

#ifdef Q_OS_WIN

#include <Windows.h>

class WindowsLibLoader : public LibLoader
{
public:
    ~WindowsLibLoader() override;

    void loadDeps(const QStringList &deps) override;
    void loadLib(const QString& lib) override;
    void* resolve(const char* symbol) override;

private:
    HMODULE _lib;
    QList<DLL_DIRECTORY_COOKIE> _deps;

    static QString getLastErrorMessage();
};

#else
//-----------------------------------------------------------------------------

QT_BEGIN_NAMESPACE
class QLibrary;
QT_END_NAMESPACE

class LinuxLibLoader : public LibLoader
{
public:
    ~LinuxLibLoader() override;
    void loadDeps(const QStringList &deps) override;
    void loadLib(const QString& lib) override;
    void* resolve(const char* symbol) override;

private:
    QLibrary* _lib = nullptr;
    QList<QLibrary*> _deps;
};

#endif

#endif // LOBLOADED_H
