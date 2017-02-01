#ifndef CK_H
#define CK_H

#include <QProcess>
#include <QStringList>

#include "appmodels.h"

class CK
{
public:
    CK();

    QList<CkEntry> getCafeeLibByUidOrAll(const QString& uid);
    QList<CkEntry> getCafeeModelByUidOrAll(const QString& uid);
    QList<ImagesDataset> getCafeeImagesByUidOrAll(const QString& uid);

    QList<CkEntry> queryCaffeLibs();
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
};

#endif // CK_H
