#ifndef CK_H
#define CK_H

#include <QProcess>
#include <QStringList>

#include "appmodels.h"

class CK
{
public:
    CK();

    QList<DnnEngine> getEnginesByUidOrAll(const QString& uid);
    QList<CkEntry> getCafeeModelByUidOrAll(const QString& uid);
    QList<ImagesDataset> getCafeeImagesByUidOrAll(const QString& uid);

    QList<DnnEngine> queryEngines();
    QList<CkEntry> queryCaffeModels();
    QList<ImagesDataset> queryCaffeImages();
    CkEntry queryEnvByUid(const QString& uid);

private:
    QProcess _ck;
    QString _ckPath;
    QStringList _args;

    QStringList ck(const QStringList &args);
    QList<CkEntry> queryEnvsByTags(const QString& tags);
    ImagesDataset loadDataset(const CkEntry& env);
    DnnEngine loadEngine(const CkEntry& env);
    QString findPackage(const QString& uoa);
};

#endif // CK_H
