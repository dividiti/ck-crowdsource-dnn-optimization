#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include <QVariant>
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

    static QString finisherFilePath();

    static QString footerRightText();
    static QString footerRightUrl();

    static int recognitionImageHeight();

    static QString styleSheet();

    static QString logPath() { return localSubdir("logs_path", "logs"); }
    static QString tmpPath() { return localSubdir("tmp_path", "tmp"); }

    static QList<Program> programs(Mode::Type mode = currentModeType());
    static QVariant currentProgram(Mode::Type mode = currentModeType());
    static void setCurrentProgram(QString uoa, Mode::Type mode = currentModeType());
    static Engine::Type currentEngineType();

    static QList<Model> models(Mode::Type mode = currentModeType(), Engine::Type engine = currentEngineType());
    static QVariant currentModel(Mode::Type mode = currentModeType(), Engine::Type engine = currentEngineType());
    static void setCurrentModel(QString uoa, Mode::Type mode = currentModeType(), Engine::Type engine = currentEngineType());

    static QList<Dataset> datasets(Mode::Type mode = currentModeType(), QVariant program = currentProgram());
    static QVariant currentDataset(Mode::Type mode = currentModeType());
    static void setCurrentDataset(QString uoa, Mode::Type mode = currentModeType());

    static QList<Mode> modes();
    static QVariant currentMode();
    static Mode::Type currentModeType();
    static void setCurrentMode(Mode::Type type);

    static int classificationStartupTimeoutSeconds();

    static qint64 fpsUpdateIntervalMs();
    static qint64 recognitionUpdateIntervalMs();

    static int batchSize();
    static void setBatchSize(int batchSize);

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
