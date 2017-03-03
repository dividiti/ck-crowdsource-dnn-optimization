#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class AppConfig
{
public:
    static int modelCount() { return sectionCount("Models"); }
    static QString modelUOA(int index) { return sectionValue("Models", index, "uoa"); }
    static QString modelName(int index) { return sectionValue("Models", index, "name"); }

    static int programCount() { return sectionCount("Programs"); }
    static QString programUOA(int index) { return sectionValue("Programs", index, "uoa"); }
    static QString programName(int index) { return sectionValue("Programs", index, "name"); }

    static int auxCount() { return sectionCount("AUX"); }
    static QString auxUOA(int index) { return sectionValue("AUX", index, "uoa"); }
    static QString auxName(int index) { return sectionValue("AUX", index, "name"); }

    static int valCount() { return sectionCount("VAL"); }
    static QString valUOA(int index) { return sectionValue("VAL", index, "uoa"); }
    static QString valName(int index) { return sectionValue("VAL", index, "name"); }

    static QString ckReposPath();
    static QString ckBinPath();
    static QString ckExeName();

    static QString styleSheetFileName();

    static QString logPath() { return localSubdir("logs_path", "logs"); }
    static QString tmpPath() { return localSubdir("tmp_path", "tmp"); }

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
