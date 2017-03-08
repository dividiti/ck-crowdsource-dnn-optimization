#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include "appmodels.h"

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class AppConfig
{
public:
    static QString ckReposPath();
    static QString ckBinPath();
    static QString ckExeName();

    static QString styleSheetFileName();

    static QString logPath() { return localSubdir("logs_path", "logs"); }
    static QString tmpPath() { return localSubdir("tmp_path", "tmp"); }

    static QList<Program> programs();
    static QVariant currentProgram();
    static void setCurrentProgram(QString uoa);

    static QList<Model> models();
    static QVariant currentModel();
    static void setCurrentModel(QString uoa);

    static QList<Dataset> datasets();
    static QVariant currentDataset();
    static void setCurrentDataset(QString uoa);

private:
    static QString configFileName();
    static QSettings& config();

    static int configValueInt(const QString& key, int defaultValue);
    static QString configValueStr(const QString& key, const QString& defaultValue);

    static QString localSubdir(const QString& key, const QString& defaultDir);

    static int sectionCount(const QString& sectionName);
    static QString sectionValue(const QString& sectionName, int index, const QString& suffix);
};


#endif // APPCONFIG_H
