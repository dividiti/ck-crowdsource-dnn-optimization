#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class AppConfig
{
public:
    static QString imagesDir();
    static QStringList imagesFilter();

    static int selectedScenarioIndex(int experimentIndex);
    static void setSelectedScenarioIndex(int experimentIndex, int scenarioIndex);

    static int selectedEngineIndex(int experimentIndex);
    static void setSelectedEngineIndex(int experimentIndex, int engineIndex);

    static int batchSize(int experimentIndex);
    static void setBatchSize(int experimentIndex, int batchSize);

    static QString ckPath();
    static QString ckBinPath();
    static QString ckExeName();
    static QStringList ckArgs();

    static bool isParallel();
    static bool logRecognitionOutput();

private:
    static QString configFileName();
    static QSettings& config();

    static int configValueInt(const QString& key, int defaultValue);
    static QString configValueStr(const QString& key, const QString& defaultValue);

    static QString experimentKey(const QString& baseKey, int experimentIndex);
};


#endif // APPCONFIG_H
