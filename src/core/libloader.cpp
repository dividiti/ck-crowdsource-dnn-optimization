#include "libloader.h"
#include "utils.h"

#include <QFile>
#include <QDebug>
#include <QDir>

LibLoader* LibLoader::create()
{
#ifdef Q_OS_WIN
    return new WindowsLibLoader;
#else
    return new LinuxLibLoader;
#endif
}

//-----------------------------------------------------------------------------

#ifdef Q_OS_WIN

WindowsLibLoader::~WindowsLibLoader()
{
    if (_lib != 0)
        FreeLibrary(_lib);

    for (auto dep: _deps)
        RemoveDllDirectory(dep);
}

void WindowsLibLoader::loadDeps(const QStringList &deps)
{
    for (const QString& path: deps)
    {
        qDebug() << "Add DLL search path" << path;
        if (!QDir(path).exists())
        {
            qWarning() << "Not found. Skipped";
            continue;
        }
        DLL_DIRECTORY_COOKIE cookie = AddDllDirectory(path.toStdWString().data());
        if (cookie == 0)
            qWarning() << "FAILED:" << getLastErrorMessage();
        _deps << cookie; // TODO: remove in destructor
    }
}

void WindowsLibLoader::loadLib(const QString& lib)
{
    const char* libpath = Utils::makeLocalStr(lib);
    _lib = LoadLibraryExA(libpath, NULL,
                             LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
                             LOAD_LIBRARY_SEARCH_DEFAULT_DIRS |
                             LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
                             LOAD_LIBRARY_SEARCH_SYSTEM32 |
                             LOAD_LIBRARY_SEARCH_USER_DIRS );
    delete[] libpath;
    if (_lib == 0)
        throw getLastErrorMessage();
}

void* WindowsLibLoader::resolve(const char* symbol)
{
    void *func = GetProcAddress(_lib, symbol);
    if (!func)
        throw QString("Unable to resolve symbol '%1': %2")
            .arg(symbol).arg(getLastErrorMessage());
    return func;
}

QString WindowsLibLoader::getLastErrorMessage()
{
    DWORD errorCode = GetLastError();
    if (errorCode == 0) return QString();

    LPSTR buffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);
    auto message = QString::fromLatin1(buffer, size);
    LocalFree(buffer);
    return message;
}

#else
//-----------------------------------------------------------------------------

#include <QLibrary>

LinuxLibLoader::~LinuxLibLoader()
{
    for (int i = _deps.size()-1; i >= 0; i--)
    {
        if (_deps.at(i)->isLoaded())
            _deps.at(i)->unload();
        delete _deps.at(i);

        if (_lib->isLoaded())
            _lib->unload();
    }
}

// Settings LD_LIBRARY_PATH inside of app does not help to search all dependencies
// (although when we set LD_LIBRARY_PATH in console before app is run, it helps).
// At least on Ubuntu. So we have to load all deps directly.
void LinuxLibLoader::loadDeps(const QStringList &deps)
{
    for (int i = deps.size()-1; i >= 0; i--)
    {
        auto depLib = deps.at(i);
        qDebug() << "Load dep lib" << i+1 << depLib;
        if (!QFile(depLib).exists())
        {
            qWarning() << "File not found";
            continue;
        }
        auto lib = new QLibrary(depLib);
        if (!lib->load())
            throw lib->errorString();
        _deps << lib;
    }
}

void LinuxLibLoader::loadLib(const QString& lib)
{
    _lib = new QLibrary(lib);
    if (!_lib->load())
        throw _lib->errorString();
}

void* LinuxLibLoader::resolve(const char* symbol)
{
    void* func = _lib->resolve(symbol);
    if (!func) throw _lib->errorString();
    return func;
}

#endif
