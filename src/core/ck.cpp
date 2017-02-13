#include "appevents.h"
#include "appconfig.h"
#include "ck.h"
#include "ckjson.h"
#include "utils.h"

#include <QFile>
#include <QDebug>
#include <QDir>

CK::CK()
{
    _reposPath = AppConfig::ckReposPath();
    if (_reposPath.isEmpty())
        AppEvents::error("CK repository path not found in config");

    auto ckDir = AppConfig::ckBinPath();
    if (ckDir.isEmpty())
        AppEvents::error("CK exe name not found in config");

#ifdef Q_OS_WIN32
    _ck.setProgram("python");
    _args = QStringList { "-W", "ignore::DeprecationWarning", ckDir + "\\..\\ck\\kernel.py" };
#else
    auto ckExe = AppConfig::ckExeName();
    if (ckExe.isEmpty())
        AppEvents::error("CK bin path not found in config");

    _ck.setProgram(ckExe);
    _ck.setWorkingDirectory(ckDir);
#endif

    qDebug() << "CK repos path:" << _reposPath;
    qDebug() << "CK bin path:" << _ck.workingDirectory();
    qDebug() << "CK executable:" << _ck.program();
    qDebug() << "CK default args:" << _args.join(" ");
}

QList<DnnModel> CK::getModelsByUidOrAll(const QString& uid)
{
    return getByUidOrAll<DnnModel>(uid, &CK::loadModel, &CK::queryModels);
}

QList<DnnEngine> CK::getEnginesByUidOrAll(const QString& uid)
{
    return getByUidOrAll<DnnEngine>(uid, &CK::loadEngine, &CK::queryEngines);
}

QList<ImagesDataset> CK::getImagesByUidOrAll(const QString& uid)
{
    return getByUidOrAll<ImagesDataset>(uid, &CK::loadImages, &CK::queryImages);
}

template <typename TResult>
QList<TResult> CK::getByUidOrAll(const QString& uid, TResult(CK::*loadResult)(const CkEntry&), QList<TResult>(CK::*getDefault)())
{
    if (uid.isEmpty())
        return (this->*getDefault)();

    auto env = queryEnvByUid(uid);
    if (env.isEmpty())
        return (this->*getDefault)();

    auto result = (this->*loadResult)(env);
    if (result.isEmpty())
        return (this->*getDefault)();

    return QList<TResult> { result };
}

QList<DnnModel> CK::queryModels()
{
    qDebug() << "----------------------------------------------";
    auto envs = queryEnvsByTags("caffemodel");
    return loadEntries<DnnModel>(envs, &CK::loadModel);
}

QList<DnnEngine> CK::queryEngines()
{
    qDebug() << "----------------------------------------------";
    auto envs = queryEnvsByTags("caffe,dnn-proxy");
    return loadEntries<DnnEngine>(envs, &CK::loadEngine);
}

QList<ImagesDataset> CK::queryImages()
{
    qDebug() << "----------------------------------------------";
    auto envs = queryEnvsByTags("dataset,caffe,aux");
    return loadEntries<ImagesDataset>(envs, &CK::loadImages);
}

template <typename TResult>
QList<TResult> CK::loadEntries(const QList<CkEntry>& envs, TResult (CK::*load)(const CkEntry&))
{
    QList<TResult> entries;
    for (auto env: envs)
    {
        auto entry = (this->*load)(env);
        if (!entry.isEmpty())
            entries << entry;
    }
    return entries;
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
        qCritical() << "No env uid found";
        return CkEntry();
    }
    auto info = CkEnvInfo(uid);
    if (!info.ok()) return CkEntry();

    auto name = info.dataName();
    if (name.isEmpty()) return CkEntry();

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

DnnModel CK::loadModel(const CkEntry &env)
{
    qDebug() << "----------------------------------------------";
    qDebug() << "Loading model info for" << env.str();
    auto meta = CkEnvMeta(env.uid);
    if (!meta.ok()) return DnnModel();

    auto weightsFile = meta.fullPath();
    if (weightsFile.isEmpty()) return DnnModel();
    qDebug() << "Weights file:" << weightsFile;

    auto modelFile = nearbyFile(weightsFile, "deploy.prototxt");
    if (modelFile.isEmpty()) return DnnModel();
    qDebug() << "Model file:" << modelFile;

    DnnModel model;
    model._title = env.name;
    model._weightsFile = weightsFile;
    model._modelFile = modelFile;
    qDebug() << "OK. Model loaded:" << env.str();
    return model;
}

ImagesDataset CK::loadImages(const CkEntry &env)
{
    qDebug() << "----------------------------------------------";
    qDebug() << "Loading dataset info for" << env.str();
    auto meta = CkEnvMeta(env.uid);
    if (!meta.ok()) return ImagesDataset();

    auto meanFile = meta.fullPath();
    if (meanFile.isEmpty()) return ImagesDataset();
    qDebug() << "Mean file:" << meanFile;

    auto labelsFile = nearbyFile(meanFile, "synset_words.txt");
    if (labelsFile.isEmpty()) return ImagesDataset();
    qDebug() << "Labels file:" << labelsFile;

    auto valFile = nearbyFile(meanFile, "val.txt");
    if (valFile.isEmpty()) return ImagesDataset();
    qDebug() << "Values file:" << valFile;

    auto tags = meta.tags();
    qDebug() << "TAGS: " + tags.join(",");
    tags.removeAll("aux");
    tags.append("raw");
    auto valEnvs = queryEnvsByTags(tags.join(","));
    if (valEnvs.isEmpty()) return ImagesDataset();
    if (valEnvs.size() > 1)
        qWarning() << "Several datasets found, take first.";

    auto valMeta = CkEnvMeta(valEnvs.first().uid);
    if (!valMeta.ok()) return ImagesDataset();

    auto imagesPath = valMeta.fullPath();
    if (imagesPath.isEmpty()) return ImagesDataset();
    imagesPath = QFileInfo(imagesPath).absolutePath();
    qDebug() << "Images path:" << imagesPath;

    ImagesDataset ds;
    ds._title = env.name;
    ds._valFile = valFile;
    ds._meanFile = meanFile;
    ds._labelsFile = labelsFile;
    ds._imagesPath = imagesPath;
    qDebug() << "OK. Dataset loaded:" << env.str();
    return ds;
}

DnnEngine CK::loadEngine(const CkEntry& env)
{
    qDebug() << "----------------------------------------------";
    qDebug() << "Loading engine info for" << env.str();
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
    qDebug() << "Engine name:" << packageName;

    auto libPath = meta.pathLib();
    auto libFile = meta.dynamicLib();
    if (libPath.isEmpty() || libFile.isEmpty()) return DnnEngine();
    auto packageLib = libPath + QDir::separator() + libFile;
    qDebug() << "Engine lib:" << packageLib;

    QMap<QString, QString> deps;
    loadDepLibs(meta, deps);
    QStringList libs(deps.values());
    libs.removeDuplicates();

    DnnEngine engine;
    engine._title = packageName;
    engine._library = packageLib;
    engine._deps = libs;
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

void CK::loadDepLibs(const CkEnvMeta& meta, QMap<QString, QString>& libs)
{
    auto setupEnvs = meta.setupEnvs();
    for (auto setupEnv: setupEnvs)
        if (setupEnv.first.contains("lib"))
        {
            auto envUid = setupEnv.second;
            if (libs.contains(envUid)) continue;

            qDebug() << "Dep found:" << setupEnv.first << setupEnv.second;
            auto envMeta = CkEnvMeta(envUid);
            auto libPath = envMeta.pathLib();
            auto libFile = envMeta.dynamicLib();
            if (libPath.isEmpty() || libFile.isEmpty())
            {
                qWarning() << "    Skipped";
                continue;
            }
            auto lib = libPath + QDir::separator() + libFile;
            if (!libs.contains(lib))
            {
                qDebug() << "    Lib:" << lib;
                libs.insert(envUid, lib);
            }
            // We need to go deeper! build full dependecies list
            loadDepLibs(envMeta, libs);
        }
}

QString CK::nearbyFile(const QString& fullFileName, const QString& nearFileName)
{
    QFileInfo f(fullFileName);
    auto nearbyFile = f.absolutePath() + QDir::separator() + nearFileName;
    if (!QFile::exists(nearbyFile))
    {
        qWarning() << "File not found:" << nearbyFile;
        return QString();
    }
    return nearbyFile;
}
