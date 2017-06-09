#include "appconfig.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <algorithm>

static const QString STYLESHEET_PATH = ":/qss/app.qss";

static Dataset createWebcamDataset() {
    Dataset ret;
    ret.auxUoa = "734ad44f6454b893";
    ret.auxName = "Webcam";
    ret.valUoa = "4ec688c967460298";
    ret.valName = "Webcam";
    ret.cmdKey = "use_webcam";
    return ret;
}

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

QString AppConfig::finisherFilePath() {
    QString ret = config().value("finisher_file", "finisher_file").toString();
    QFileInfo info(ret);
    qWarning() << "absolute finisher path: " << info.absoluteFilePath();
    return info.absoluteFilePath();
}

QString AppConfig::footerRightText() {
    return config().value("footer_right_text", "").toString();
}

QString AppConfig::footerRightUrl() {
    return config().value("footer_right_url", "").toString();
}

int AppConfig::recognitionImageHeight() {
    return config().value("recognition_image_height", -1).toInt();
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

QList<Program> AppConfig::programs(Mode::Type mode) {
    const QString section = Mode::Type::CLASSIFICATION == mode ? "Programs" : "DetectionPrograms";
    int programCount = sectionCount(section);
    QList<Program> ret;
    for (int i = 0; i < programCount; ++i) {
        auto outputFile = sectionValue(section, i, "output_file");
        auto exe = sectionValue(section, i, "exe");
        auto programPath = sectionValue(section, i, "path");
        auto programUoa = sectionValue(section, i, "uoa");
        bool webcam = "1" == sectionValue(section, i, "webcam");
        auto programEngine = Engine::parse(sectionValue(section, i, "engine"));
        int targetCount = configValueInt(section + "/" + QString::number(i) + "_target_count", 0);
        for (int j = 0; j < targetCount; ++j) {
            auto keyPrefix = "target_" + QString::number(j) + "_";
            Program p;
            p.name = sectionValue(section, i, keyPrefix + "name");
            p.targetUoa = sectionValue(section, i, keyPrefix + "uoa");
            p.programUoa = programUoa;
            p.engine = programEngine;
            p.supportsWebcam = webcam;

            auto targetPath = sectionValue(section, i, keyPrefix + "path");
            auto targetFullPath = pathAppend(programPath, targetPath);

            p.targetDir = targetPath;
            p.outputFile = pathAppend(targetFullPath, outputFile);
            p.exe = pathAppend(targetFullPath, exe);

            if ("squeezedet" == programUoa || isCompiled(p)) {
                ret.append(p);
            }
        }
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}

static QString currentProgramKey(Mode::Type mode) {
    return Mode::Type::CLASSIFICATION == mode ? "classification_target_uoa" : "recognition_target_uoa";
}

QVariant AppConfig::currentProgram(Mode::Type mode) {
    QString uoa = configValueStr(currentProgramKey(mode), "");
    QList<Program> progs = programs(mode);
    QVariant ret;
    for (auto i : progs) {
        if (!ret.isValid()) {
            ret.setValue(i);
        }
        if (i.targetUoa == uoa) {
            ret.setValue(i);
            break;
        }
    }
    return ret;
}

void AppConfig::setCurrentProgram(QString uoa, Mode::Type mode) {
    config().setValue(currentProgramKey(mode), uoa);
    config().sync();
}

QList<Model> AppConfig::models(Mode::Type mode, Engine::Type engine) {
    const QString section = Mode::Type::CLASSIFICATION == mode ? "Models" : "DetectionModels";
    int count = sectionCount(section);
    QList<Model> ret;
    for (int i = 0; i < count; ++i) {
        Model m;
        m.engine = Engine::parse(sectionValue(section, i, "engine"));
        if (engine != m.engine) {
            continue;
        }
        m.uoa = sectionValue(section, i, "uoa");
        m.name = sectionValue(section, i, "name");
        ret.append(m);
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}

static QString currentModelKey(Mode::Type mode, Engine::Type engine) {
    QString prefix = Mode::Type::CLASSIFICATION == mode ? "classification_model_uoa" : "recognition_model_uoa";
    return prefix + "_" + Engine::toString(engine);
}

QVariant AppConfig::currentModel(Mode::Type mode, Engine::Type engine) {
    QString uoa = configValueStr(currentModelKey(mode, engine), "");
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

void AppConfig::setCurrentModel(QString uoa, Mode::Type mode, Engine::Type engine) {
    config().setValue(currentModelKey(mode, engine), uoa);
    config().sync();
}

QList<Dataset> AppConfig::datasets(Mode::Type mode, QVariant program) {
    const QString valSection = Mode::Type::CLASSIFICATION == mode ? "VAL" : "DetectionDatasets";
    const QString auxSection = "AUX";
    int valCount = sectionCount(valSection);
    int auxCount = sectionCount(auxSection);
    QList<Dataset> ret;
    if (program.isValid() && program.value<Program>().supportsWebcam) {
        ret.append(createWebcamDataset());
    }
    if (0 >= valCount) {
        return ret;
    }
    for (int i = 0; i < valCount; ++i) {
        Dataset m;
        m.valUoa = sectionValue(valSection, i, "uoa");
        m.valName = sectionValue(valSection, i, "name");

        bool append = true;

        if (Mode::Type::CLASSIFICATION == mode) {
            QString auxPackageUoa = sectionValue(valSection, i, "aux_package_uoa");
            for (int j = 0; j < auxCount; ++j) {
                if (auxPackageUoa == sectionValue(auxSection, j, "package_uoa")) {
                    m.auxUoa = sectionValue(auxSection, j, "uoa");
                    m.auxName = sectionValue(auxSection, j, "name");
                    break;
                }
            }
            if (m.auxUoa.isEmpty()) {
                qWarning() << "AUX not found for VAL " + m.valName + " (" + m.valUoa + "), skipping";
                append = false;
            }
        }

        if (append) {
            ret.append(m);
        }
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}

static QString currentDatasetKey(Mode::Type mode) {
    return Mode::Type::CLASSIFICATION == mode ? "classification_val_uoa" : "recognition_val_uoa";
}

QVariant AppConfig::currentDataset(Mode::Type mode) {
    QString uoa = configValueStr(currentDatasetKey(mode), "");
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

void AppConfig::setCurrentDataset(QString uoa, Mode::Type mode) {
    config().setValue(currentDatasetKey(mode), uoa);
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

Mode::Type AppConfig::currentModeType() {
    return currentMode().value<Mode>().type;
}

Engine::Type AppConfig::currentEngineType() {
    QVariant p = currentProgram();
    return p.isValid() ? p.value<Program>().engine : Engine::Type::UNKNOWN;
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
    return config().value("recognition_update_interval_ms", 10).toInt();
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
