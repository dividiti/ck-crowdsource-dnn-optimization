#include "appevents.h"
#include "appconfig.h"
#include "ck.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

CK::CK()
{
    _ckPath = AppConfig::ckPath();
#ifdef Q_OS_WIN32
    _ck.setProgram("python");
    _args = QStringList { "-W", "ignore::DeprecationWarning", AppConfig::ckBinPath()+"\\..\\ck\\kernel.py" };
#else
    _ck.setProgram(AppConfig::ckExeName());
    _ck.setWorkingDirectory(AppConfig::ckBinPath());
#endif

    qDebug() << "CK path:" << _ckPath;
    qDebug() << "CK bin path:" << _ck.workingDirectory();
    qDebug() << "CK executable:" << _ck.program();
    qDebug() << "CK default args:" << _ck.arguments().join(" ");
}

QList<CkEntry> CK::queryCaffeLibs()
{
    return queryEnvsByTags("lib,caffe");
}

QList<CkEntry> CK::queryCaffeModels()
{
    return queryEnvsByTags("caffemodel");
}

QList<CkEntry> CK::queryEnvsByTags(const QString& tags)
{
    auto args = QStringList{ "search", "env", "--tags="+tags };
    qDebug() << "Run command:" << _ck.program() + ' ' +  args.join(" ");
    auto results = ck(args);
    if (results.isEmpty())
    {
        qDebug() << "No envs found";
        return QList<CkEntry>();
    }
    for (auto result: results)
        qDebug() << "Search result:" << result;
    QList<CkEntry> entries;
    qDebug() << "Parsing results...";
    for (auto result: results)
    {
        // result = "local:env:fff6cd1bb4dc78f2"
        auto uid = result.section(':', -1);
        auto entry = queryEnvByUid(uid);
        if (!entry.isEmpty())
            entries << entry;
    }
    return entries;
}

CkEntry CK::queryEnvByUid(const QString& uid)
{
    if (uid.isEmpty())
    {
        qCritical() << "No env uid founded";
        return CkEntry();
    }
    auto info = makePath({ _ckPath, "local", "env", uid, ".cm", "info.json" });
    auto json = QJsonDocument::fromJson(Utils::loadTtextFromFile(info));
    auto name = json.object()["data_name"].toString();
    if (name.isEmpty())
        return CkEntry();
    CkEntry entry { uid, name };
    qDebug() << "Env found:" << entry.str();
    return entry;
}

QStringList CK::ck(const QStringList& args)
{
#ifdef Q_OS_WIN32
    QStringList fullArgs = _args;
    fullArgs.append(args);
    _ck.setArguments(fullArgs);
#else
    _ck.setArguments(args);
#endif
    _ck.start();
    _ck.waitForFinished();
    auto error = _ck.errorString();
    auto errors = QString::fromLatin1(_ck.readAllStandardError());
    auto output = QString::fromLatin1(_ck.readAllStandardOutput());
    if (output.isEmpty() && (_ck.error() != QProcess::UnknownError || !errors.isEmpty()))
    {
        AppEvents::error(QString("Error running program %1/%2: %3\n%4")
            .arg(_ck.workingDirectory()).arg(_ck.program()).arg(error).arg(errors));
    }
    return output.split("\n", QString::SkipEmptyParts);
}

QString CK::makePath(const QStringList &parts) const
{
    return parts.join(QDir::separator());
}
