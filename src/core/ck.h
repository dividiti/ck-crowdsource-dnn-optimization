#ifndef CK_H
#define CK_H

#include <QProcess>
#include <QStringList>

#include "appmodels.h"

class CK
{
public:
    CK();

    QList<CkEntry> queryCaffeLibs();
    QList<CkEntry> queryCaffeModels();
    CkEntry queryEnvByUid(const QString& uid);

private:
    QProcess _ck;
    QString _ckPath;
    QStringList _args;

    QStringList ck(const QStringList &args);
    QString makePath(const QStringList &parts) const;
    QList<CkEntry> queryEnvsByTags(const QString& tags);
};

#endif // CK_H
