#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class AppConfig
{
public:
    static int selectedModelIndex(int experimentIndex);
    static void setSelectedModelIndex(int experimentIndex, int modelIndex);

    static int selectedEngineIndex(int experimentIndex);
    static void setSelectedEngineIndex(int experimentIndex, int engineIndex);

    static int selectedImagesIndex(int experimentIndex);
    static void setSelectedImagesIndex(int experimentIndex, int imagesIndex);

    static int batchSize(int experimentIndex);
    static void setBatchSize(int experimentIndex, int batchSize);

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

    static QString experimentKey(const QString& baseKey, int experimentIndex);

    static QString localSubdir(const QString& key, const QString& defaultDir);
};


#endif // APPCONFIG_H
