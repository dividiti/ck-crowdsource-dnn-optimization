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
}

QList<CkEntry> CK::queryCaffeModels()
{
    QList<CkEntry> models;
    qDebug() << "Query caffe models...";
    for (auto result: ck({ "search", "env", "--tags=caffemodel" }))
    {
        // result = "local:env:fff6cd1bb4dc78f2"
        auto uid = result.section(':', -1);
        if (uid.isEmpty()) continue;
        auto info = makePath({ _ckPath, "local", "env", uid, ".cm", "info.json" });
        auto json = QJsonDocument::fromJson(Utils::loadTtextFromFile(info));
        auto name = json.object()["data_name"].toString();
        if (name.isEmpty()) continue;
        qDebug() << "Model env found:" << uid << name;
        models << CkEntry { uid, name };
    }
    return models;
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
