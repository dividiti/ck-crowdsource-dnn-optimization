#include "appconfig.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

static const QString STYLESHEET_PATH = ":/qss/app.qss";

QSettings& AppConfig::config() {
    static QSettings cfg(configFileName(), QSettings::IniFormat);
    return cfg;
}

QString AppConfig::configFileName() {
    return qApp->applicationDirPath() + QDir::separator() + "app.conf";
}

QString AppConfig::styleSheet() {
    const QString path = STYLESHEET_PATH;
    QFile file(path);
    if (!file.exists()) {
        qWarning() << "Stylesheet file not found: " << path;
        return "";
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "unable to open stylesheet file: " << file.errorString();
        return "";
    }
    return QTextStream(&file).readAll();
}

QString AppConfig::ckReposPath() {
    return config().value("ck_repos_path").toString();
}

QString AppConfig::ckBinPath() {
    return config().value("ck_bin_path").toString();
}

QString AppConfig::ckExeName() {
    return config().value("ck_exe_name").toString();
}

int AppConfig::configValueInt(const QString& key, int defaultValue) {
    bool ok;
    int value = config().value(key, defaultValue).toInt(&ok);
    return ok? value: defaultValue;
}

QString AppConfig::configValueStr(const QString& key, const QString& defaultValue){
    auto value = config().value(key, defaultValue).toString();
    return value.isEmpty()? defaultValue: value;
}

QString AppConfig::localSubdir(const QString& key, const QString& defaultDir) {
    auto defaultPath = qApp->applicationDirPath() + QDir::separator() + defaultDir;
    auto path = configValueStr(key, defaultPath);
    QDir d(path);
    if (!d.exists()) {
        d.mkpath(path);
    }
    return d.absolutePath();
}

int AppConfig::sectionCount(const QString& sectionName) {
    return configValueInt(sectionName + "/count", 0);
}

QString AppConfig::sectionValue(const QString& sectionName, int index, const QString& suffix) {
    return configValueStr(sectionName + "/" + QString::number(index) + "_" + suffix, "");
}

bool isCompiled(const Program& p) {
    QFileInfo out(p.outputFile);
    QDir dir = out.absoluteDir();
    return dir.exists(p.exe);
}

static QString pathAppend(const QString& path1, const QString& path2) {
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

QList<Program> AppConfig::programs() {
    const QString section = "Programs";
    int programCount = sectionCount(section);
    QList<Program> ret;
    for (int i = 0; i < programCount; ++i) {
        auto outputFile = sectionValue(section, i, "output_file");
        auto exe = sectionValue(section, i, "exe");
        auto programPath = sectionValue(section, i, "path");
        auto programUoa= sectionValue(section, i, "uoa");
        int targetCount = configValueInt(section + "/" + QString::number(i) + "_target_count", 0);
        for (int j = 0; j < targetCount; ++j) {
            auto keyPrefix = "target_" + QString::number(j) + "_";
            Program p;
            p.name = sectionValue(section, i, keyPrefix + "name");
            p.target_uoa = sectionValue(section, i, keyPrefix + "uoa");
            p.program_uoa = programUoa;

            auto targetPath = sectionValue(section, i, keyPrefix + "path");
            auto targetFullPath = pathAppend(programPath, targetPath);

            p.target_dir = targetPath;
            p.outputFile = pathAppend(targetFullPath, outputFile);
            p.exe = pathAppend(targetFullPath, exe);

            if (isCompiled(p)) {
                ret.append(p);
            }
        }
    }
    return ret;
}

QVariant AppConfig::currentProgram() {
    QString uoa = configValueStr("classification_target_uoa", "");
    QList<Program> progs = programs();
    QVariant ret;
    for (auto i : progs) {
        if (!ret.isValid()) {
            ret.setValue(i);
        }
        if (i.target_uoa == uoa) {
            ret.setValue(i);
            break;
        }
    }
    return ret;
}

void AppConfig::setCurrentProgram(QString uoa) {
    config().setValue("classification_target_uoa", uoa);
    config().sync();
}

QVariant AppConfig::currentSqueezeDetProgram() {
    static const QString& SECTION = "SqueezeDet";
    int programCount = sectionCount(SECTION);
    if (programCount < 1) {
        return QVariant();
    }
    Program p;
    p.program_uoa = sectionValue(SECTION, 0, "uoa");
    p.name = sectionValue(SECTION, 0, "name");
    p.outputFile = sectionValue(SECTION, 0, "output_file");
    p.exe = sectionValue(SECTION, 0, "exe");
    p.target_dir = "tmp";
    QVariant ret;
    ret.setValue(p);
    return ret;
}

QList<Model> AppConfig::models() {
    int count = sectionCount("Models");
    QList<Model> ret;
    for (int i = 0; i < count; ++i) {
        Model m;
        m.uoa = sectionValue("Models", i, "uoa");
        m.name = sectionValue("Models", i, "name");
        ret.append(m);
    }
    return ret;
}

QVariant AppConfig::currentModel() {
    QString uoa = configValueStr("model_uoa", "");
    QVariant ret;
    for (auto i : models()) {
        if (!ret.isValid()) {
            ret.setValue(i);
        }
        if (i.uoa == uoa) {
            ret.setValue(i);
            break;
        }
    }
    return ret;
}

void AppConfig::setCurrentModel(QString uoa) {
    config().setValue("model_uoa", uoa);
    config().sync();
}

QList<Dataset> AppConfig::datasets() {
    int valCount = sectionCount("VAL");
    int auxCount = sectionCount("AUX");
    QList<Dataset> ret;
    if (0 >= valCount || 0 >= auxCount) {
        return ret;
    }
    for (int i = 0; i < valCount; ++i) {
        Dataset m;
        m.valUoa = sectionValue("VAL", i, "uoa");
        m.valName = sectionValue("VAL", i, "name");

        QString auxPackageUoa = sectionValue("VAL", i, "aux_package_uoa");
        for (int j = 0; j < auxCount; ++j) {
            if (auxPackageUoa == sectionValue("AUX", j, "package_uoa")) {
                m.auxUoa = sectionValue("AUX", j, "uoa");
                m.auxName = sectionValue("AUX", j, "name");
                break;
            }
        }

        if (m.auxUoa.isEmpty()) {
            qWarning() << "AUX not found for VAL " + m.valName + " (" + m.valUoa + "), skipping";
        } else {
            ret.append(m);
        }
    }
    return ret;
}

QVariant AppConfig::currentDataset() {
    QString uoa = configValueStr("val_uoa", "");
    QVariant ret;
    for (auto i : datasets()) {
        if (!ret.isValid()) {
            ret.setValue(i);
        }
        if (i.valUoa == uoa) {
            ret.setValue(i);
            break;
        }
    }
    return ret;
}

void AppConfig::setCurrentDataset(QString uoa) {
    config().setValue("val_uoa", uoa);
    config().sync();
}

QList<Mode> AppConfig::modes() {
    return QList<Mode>({ Mode(Mode::Type::CLASSIFICATION), Mode(Mode::Type::RECOGNITION) });
}

QVariant AppConfig::currentMode() {
    Mode m(static_cast<Mode::Type>(configValueInt("demo_mode", Mode::Type::CLASSIFICATION)));
    QVariant ret;
    ret.setValue(Mode());
    for (auto m1 : modes()) {
        if (m.type == m1.type) {
            ret.setValue(m);
            break;
        }
    }
    return ret;
}

void AppConfig::setCurrentMode(Mode::Type type) {
    for (auto m : modes()) {
        if (m.type == type) {
            config().setValue("demo_mode", type);
            config().sync();
            return;
        }
    }
}

int AppConfig::classificationStartupTimeoutSeconds() {
    return config().value("startup_timeout_seconds", 40).toInt();
}

qint64 AppConfig::fpsUpdateIntervalMs() {
    return config().value("fps_update_interval_ms", 500).toInt();
}

qint64 AppConfig::recognitionUpdateIntervalMs() {
    return config().value("recognition_update_interval_ms", 1000).toInt();
}

int AppConfig::batchSize() {
    int ret = config().value("batch_size", 1).toInt();
    if (0 >= ret) {
        qWarning() << "Non-positive batch size is specified in the config file. Defaulting to 1. Batch size from config: " << ret;
        ret = 1;
    }
    return ret;
}

void AppConfig::setBatchSize(int batchSize) {
    if (0 >= batchSize) {
        qWarning() << "Trying to set non-positive batch size, which is not allowed: " << batchSize;
        return;
    }
    config().setValue("batch_size", batchSize);
    config().sync();
}
