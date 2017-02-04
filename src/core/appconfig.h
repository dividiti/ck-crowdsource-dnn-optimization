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

    static int selectedModelIndex(int experimentIndex);
    static void setSelectedModelIndex(int experimentIndex, int modelIndex);

    static int selectedEngineIndex(int experimentIndex);
    static void setSelectedEngineIndex(int experimentIndex, int engineIndex);

    static int selectedImagesIndex(int experimentIndex);
    static void setSelectedImagesIndex(int experimentIndex, int imagesIndex);

    static int batchSize(int experimentIndex);
    static void setBatchSize(int experimentIndex, int batchSize);

    static QString ckPath();
    static QString ckBinPath();
    static QString ckExeName();
    static QStringList ckArgs();

    static QString styleSheetFileName();

    static QString logPath();

private:
    static QString configFileName();
    static QSettings& config();

    static int configValueInt(const QString& key, int defaultValue);
    static QString configValueStr(const QString& key, const QString& defaultValue);

    static QString experimentKey(const QString& baseKey, int experimentIndex);
};


#endif // APPCONFIG_H
