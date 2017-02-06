#ifndef CK_H
#define CK_H

#include <QProcess>
#include <QStringList>

#include "appmodels.h"

class CkEntry
{
public:
    QString uid;
    QString name;

    QString str() const { return QString("%1: %2").arg(uid, name); }

    bool isEmpty() const { return uid.isEmpty(); }
};

//-----------------------------------------------------------------------------

class CK
{
public:
    CK();

    QList<DnnModel> getModelsByUidOrAll(const QString& uid);
    QList<DnnEngine> getEnginesByUidOrAll(const QString& uid);
    QList<ImagesDataset> getImagesByUidOrAll(const QString& uid);

    QList<DnnModel> queryModels();
    QList<DnnEngine> queryEngines();
    QList<ImagesDataset> queryImages();

    CkEntry queryEnvByUid(const QString& uid);

private:
    QProcess _ck;
    QString _ckPath;
    QStringList _args;

    DnnModel loadModel(const CkEntry& env);
    DnnEngine loadEngine(const CkEntry& env);
    ImagesDataset loadImages(const CkEntry& env);

    QStringList ck(const QStringList &args);
    QList<CkEntry> queryEnvsByTags(const QString& tags);
    QString findPackage(const QString& uoa);
    QStringList loadDepLibs(const class CkEnvMeta& meta);
    QString nearbyFile(const QString& fullFileName, const QString& nearFileName);

    template <typename TResult>
    QList<TResult> getByUidOrAll(const QString& uid, TResult(CK::*loadResult)(const CkEntry&), QList<TResult>(CK::*getDefault)());
    template <typename TResult>
    QList<TResult> loadEntries(const QList<CkEntry>& envs, TResult (CK::*load)(const CkEntry&));
};

#endif // CK_H
