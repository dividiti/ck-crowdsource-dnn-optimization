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
    _ck.setWorkingDirectory(AppConfig::ckBinPath());
    _ck.setProgram(AppConfig::ckExeName());
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
    _ck.setArguments(args);
    _ck.start();
    _ck.waitForFinished();
    return QString::fromLatin1(_ck.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
}

QString CK::makePath(const QStringList &parts) const
{
    return parts.join(QDir::separator());
}
