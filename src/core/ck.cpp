#include "appevents.h"
#include "appconfig.h"
#include "ck.h"
#include "ckjson.h"
#include "utils.h"

#include <QFile>
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

CK::CK()
{
    _ckPath = AppConfig::ckPath();
    // TODO: same initialization as in ScenarioRuner, should be merged
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

QList<DnnEngine> CK::getEnginesByUidOrAll(const QString& uid)
{
    if (uid.isEmpty())
        return queryEngines();

    auto env = queryEnvByUid(uid);
    if (env.isEmpty())
        return queryEngines();

    auto engine = loadEngine(env);
    if (engine.isEmpty())
        return queryEngines();

    return QList<DnnEngine> { engine };
}

QList<CkEntry> CK::getCafeeModelByUidOrAll(const QString& uid)
{
    if (uid.isEmpty())
        return queryCaffeModels();

    auto env = queryEnvByUid(uid);
    if (env.isEmpty())
        return queryCaffeModels();

    return QList<CkEntry> { env };
}

QList<ImagesDataset> CK::getCafeeImagesByUidOrAll(const QString& uid)
{
    if (uid.isEmpty())
        return queryCaffeImages();

    auto env = queryEnvByUid(uid);
    if (env.isEmpty())
        return queryCaffeImages();

    auto images = loadDataset(env);
    if (images.isEmpty())
        return queryCaffeImages();

    return QList<ImagesDataset> { images };
}

QList<DnnEngine> CK::queryEngines()
{
    QList<DnnEngine> engines;
    auto envs = queryEnvsByTags("lib,dnn-proxy");
    for (auto env: envs)
    {
        auto engine = loadEngine(env);
        if (!engine.isEmpty())
            engines << engine;
    }
    return engines;
}

QList<CkEntry> CK::queryCaffeModels()
{
    return queryEnvsByTags("caffemodel");
}

QList<ImagesDataset> CK::queryCaffeImages()
{
    QList<ImagesDataset> datasets;
    auto envs = queryEnvsByTags("dataset,caffe,aux");
    for (auto env: envs)
    {
        auto dataset = loadDataset(env);
        if (!dataset.isEmpty())
            datasets << dataset;
    }
    return datasets;
}

QList<CkEntry> CK::queryEnvsByTags(const QString& tags)
{
    auto args = QStringList{ "search", "env", "--tags="+tags };
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
    auto info = Utils::makePath({ _ckPath, "local", "env", uid, ".cm", "info.json" });
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
    static QString errorMarker("CK error:");

#ifdef Q_OS_WIN32
    QStringList fullArgs = _args;
    fullArgs.append(args);
    _ck.setArguments(fullArgs);
#else
    _ck.setArguments(args);
#endif
    qDebug() << "Run CK command:" << _ck.program() + ' ' +  _ck.arguments().join(" ");
    _ck.start();
    _ck.waitForFinished();
    auto error = _ck.errorString();
    auto errors = QString::fromLatin1(_ck.readAllStandardError());
    auto output = QString::fromLatin1(_ck.readAllStandardOutput());
//    qDebug() << "STDERR: " + errors;
//    qDebug() << "STDOUT: " + output;
    if (output.isEmpty() && (_ck.error() != QProcess::UnknownError || !errors.isEmpty()))
    {
        AppEvents::error(QString("Error running program %1/%2: %3\n%4")
            .arg(_ck.workingDirectory()).arg(_ck.program()).arg(error).arg(errors));
        return QStringList();
    }
    auto lines = output.split("\n", QString::SkipEmptyParts);
    for (const QString& line: lines)
        if (line.startsWith(errorMarker))
        {
            AppEvents::error(output);
            return QStringList();
        }
    return lines;
}

ImagesDataset CK::loadDataset(const CkEntry &env)
{
    static QString keyImagenetVal("CK_CAFFE_IMAGENET_VAL_TXT");
    static QString keyImagenetWords("CK_CAFFE_IMAGENET_SYNSET_WORDS_TXT");
    static QString keyImagenetPath("CK_CAFFE_IMAGENET_VAL");

    qDebug() << "Loading dataset info for" << env.str();
    auto meta = CkEnvMeta(env.uid);
    if (!meta.ok()) return ImagesDataset();

    auto valFile = meta.envVar(keyImagenetVal);
    auto wordsFile = meta.envVar(keyImagenetWords);
    qDebug() << keyImagenetVal + ": " + valFile;
    qDebug() << keyImagenetWords + ": " + wordsFile;

    auto tags = meta.tags();
    qDebug() << "TAGS: " + tags.join(",");
    tags.removeAll("aux");
    tags.append("raw");
    auto envs = queryEnvsByTags(tags.join(","));
    if (envs.isEmpty()) return ImagesDataset();
    if (envs.size() > 1)
        qWarning() << "Several datasets found, take first.";

    meta = CkEnvMeta(envs.first().uid);
    if (!meta.ok()) return ImagesDataset();

    auto imagesPath = meta.envVar(keyImagenetPath);
    qDebug() << keyImagenetPath + ": " + imagesPath;

    ImagesDataset ds;
    ds._title = env.name;
    ds._valFile = valFile;
    ds._wordsFile = wordsFile;
    ds._imagesPath = imagesPath;
    qDebug() << "OK. Dataset loaded:" << env.str();
    return ds;
}

DnnEngine CK::loadEngine(const CkEntry& env)
{
    qDebug() << "Loading engine info for " << env.str();
    auto meta = CkEnvMeta(env.uid);
    if (!meta.ok()) return DnnEngine();

    auto packageUoa = meta.packageUoa();
    if (packageUoa.isEmpty()) return DnnEngine();

    auto packagePath = findPackage(packageUoa);
    if (packagePath.isEmpty()) return DnnEngine();

    auto packageInfo = CkInfo(packagePath);
    if (!packageInfo.ok()) return DnnEngine();

    auto packageName = packageInfo.dataName();
    if (packageName.isEmpty()) return DnnEngine();
    qDebug() << "Engine name: " + packageName;

    auto libPath = meta.valueStr({"customize", "path_lib"});
    auto libFile = meta.valueStr({"customize", "dynamic_lib"});
    if (libPath.isEmpty() || libFile.isEmpty()) return DnnEngine();
    auto packageLib = libPath + QDir::separator() + libFile;
    qDebug() << "Engine lib: " + packageLib;

    DnnEngine engine;
    engine._title = packageName;
    engine._libFile = packageLib;
    qDebug() << "OK. Engine loaded:" << env.uid << engine.title();
    return engine;
}

QString CK::findPackage(const QString& uoa)
{
    auto paths = ck({ "find", "package:" + uoa });
    if (paths.isEmpty())
    {
        qDebug() << "Packages not found";
        return QString();
    }
    for (auto path: paths)
        qDebug() << path;
    if (paths.size() > 1)
        qWarning() << "Several packages found, take first.";
    return paths.first();
}
