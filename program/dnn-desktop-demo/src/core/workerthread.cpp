#include "workerthread.h"
#include "appconfig.h"
#include "appevents.h"

#include <QProcess>
#include <QTextStream>
#include <QRegExp>
#include <QDebug>
#include <QFile>
#include <QDateTime>

static QString getExe() {
    QString ckExe = AppConfig::ckExeName();
    if (ckExe.isEmpty()) {
        AppEvents::error("CK bin path not found in config");
    }
    return ckExe;
}

static QString getBinPath() {
    auto ckDir = AppConfig::ckBinPath();
    if (ckDir.isEmpty()) {
        AppEvents::error("CK exe name not found in config");
    }
    return ckDir;
}

static const QString FILE_PREFIX = "File: ";
static const QString ORIGINAL_FILE_PREFIX = "Original file: ";
static const QString DURATION_PREFIX = "Duration: ";
static const QString DURATION_SUFFIX = " sec";
static const QString CORRECT_LABEL_PREFIX = "Correct label: ";
static const QString PREDICTION_PREFIX = "Predictions: ";
static const QRegExp PREDICTION_REGEX("([0-9]*\\.?[0-9]+) - \"([^\"]+)\"");

static const QRegExp RECOGNIZED_OBJECT_REGEX("Recognized ([^:]*): (\\d+)");
static const QRegExp EXPECTED_OBJECT_REGEX("Expected ([^:]*): (\\d+)");
static const QRegExp FALSE_POSITIVE_OBJECT_REGEX("False positive ([^:]*): (\\d+)");

static const QRegExp DETECTION_REGEX("Detection ([^:]*): ([0-9.]+) ([0-9.]+) ([0-9.]+) ([0-9.]+) ([0-9.]+)");
static const QRegExp GROUND_TRUTH_REGEX("Ground truth ([^:]*): ([0-9.]+) ([0-9.]+) ([0-9.]+) ([0-9.]+) ([0-9.]+)");

static const QRegExp ROLLING_AP_REGEX("Rolling AP ([^:]*): ([0-9.]+) easy, ([0-9.]+) moderate, ([0-9.]+) hard");
static const QRegExp ROLLING_MEAN_AP_REGEX("Rolling mAP: ([0-9.]+)");

static const long NORMAL_WAIT_MS = 50;
static const long KILL_WAIT_MS = 1000 * 10;

static ImageObject objectFromMatch(const QRegExp& m) {
    ImageObject ret;
    ret.label = m.cap(1).trimmed();
    ret.xmin = m.cap(2).toFloat();
    ret.ymin = m.cap(3).toFloat();
    ret.xmax = m.cap(4).toFloat();
    ret.ymax = m.cap(5).toFloat();
    ret.score = m.cap(6).toFloat();
    return ret;
}

WorkerThread::WorkerThread(const Program& program, const Mode& mode, QObject* parent) : QThread(parent), program(program), mode(mode) {}

QStringList WorkerThread::getArgs() {
    QStringList ret;
    switch (mode.type) {
    case Mode::Type::RECOGNITION:
        ret = QStringList {
            "run",
            "program:" + program.programUoa,
            "--tmp_dir=" + program.targetDir,
            "--cmd_key=" + dataset.cmdKey,
            "--deps.caffemodel=" + model.uoa,
            "--deps.squeezedet=" + model.uoa,
            "--deps.detection-dataset=" + dataset.valUoa,
            "--deps.lib-tensorflow=" + program.targetUoa
            };
        if (!skipFilesIncluding.isEmpty()) {
            ret.append("--env.SKIP_FILES_INCLUDING=" + skipFilesIncluding);
        }
        break;

    case Mode::Type::CLASSIFICATION:
    default:
        ret = QStringList {
            "run",
            "program:" + program.programUoa,
            "--tmp_dir=" + program.targetDir,
            "--cmd_key=" + dataset.cmdKey,
            "--deps.caffemodel=" + model.uoa,
            "--deps.imagenet-aux=" + dataset.auxUoa,
            "--deps.imagenet-val=" + dataset.valUoa,
            "--env.CK_CAFFE_BATCH_SIZE=" + QString::number(batchSize)
            };
    }

    for (auto e: dataset.env.toStdMap()) {
        ret.append("--env." + e.first + "=" + e.second);
    }

    ret.append("--env.FINISHER_FILE=" + AppConfig::finisherFilePath());
    ret.append("--quiet");
    return ret;
}

static void insertOrUpdate(QMap<QString, int>& map, QString key, int v) {
    if (map.contains(key)) {
        map[key] += v;
    } else {
        map[key] = v;
    }
}

void WorkerThread::run() {
    normalExit = false;

    QProcess ck;
    ck.setWorkingDirectory(getBinPath());
    ck.setProgram(getExe());
    ck.setArguments(getArgs());

    const QString runCmd = ck.program() + " " +  ck.arguments().join(" ");
    qDebug() << "Run CK command: " << runCmd;

    QFile outputFile(program.outputFile);
    outputFile.remove();

    QFile finisherFile(AppConfig::finisherFilePath());
    finisherFile.remove();

    ck.start();

    long timout = 1000 * AppConfig::classificationStartupTimeoutSeconds();
    qDebug() << "Waiting until the program starts writing data to " << program.outputFile;
    while (!outputFile.exists() && !isInterruptionRequested()) {
        if (ck.waitForFinished(NORMAL_WAIT_MS)) {
            AppEvents::error("Program stopped prematurely. "
                             "Please, select the command below, copy it and run manually from command line "
                             "to investigate the issue:\n\n" + runCmd);
            emitStopped();
            return;
        }
        timout -= NORMAL_WAIT_MS;
        if (0 >= timout) {
            emitStopped();
            ck.kill();
            ck.waitForFinished(KILL_WAIT_MS);
            AppEvents::error("Program startup takes too long. "
                             "Please, select the command below, copy it and run manually from command line "
                             "to investigate the issue:\n\n" + runCmd);
            return;
        }
    }

    qDebug() << "Starting reading data";
    outputFile.open(QIODevice::ReadOnly);

    QTextStream stream(&outputFile);
    QString line;
    ImageResult ir;
    int predictionCount = 0;
    bool finished = false;
    while (!isInterruptionRequested()) {
        line = stream.readLine();
        if (line.isNull()) {
            if (finished) {
                break;
            }
            finished = ck.waitForFinished(NORMAL_WAIT_MS);
            continue;
        }
        line = line.trimmed();
        if (line.isEmpty()) {
            processPredictedResults(ir);
            ir = ImageResult();

        } else if (line.startsWith(FILE_PREFIX)) {
            ir.imageFile = line.mid(FILE_PREFIX.size());

        } else if (line.startsWith(ORIGINAL_FILE_PREFIX)) {
            ir.originalImageFile = line.mid(ORIGINAL_FILE_PREFIX.size());

        } else if (line.startsWith(DURATION_PREFIX)) {
            QStringRef t = line.midRef(DURATION_PREFIX.size());
            t = t.left(t.size() - DURATION_SUFFIX.size());
            ir.duration = t.toDouble();

        } else if (line.startsWith(CORRECT_LABEL_PREFIX)) {
            ir.correctLabels = line.mid(CORRECT_LABEL_PREFIX.size()).trimmed();

        } else if (line.startsWith(PREDICTION_PREFIX)) {
            predictionCount = line.mid(PREDICTION_PREFIX.size()).toInt();

        } else if (RECOGNIZED_OBJECT_REGEX.exactMatch(line)) {
            insertOrUpdate(ir.recognizedObjects, RECOGNIZED_OBJECT_REGEX.cap(1).trimmed(), RECOGNIZED_OBJECT_REGEX.cap(2).toInt());

        } else if (EXPECTED_OBJECT_REGEX.exactMatch(line)) {
            insertOrUpdate(ir.expectedObjects, EXPECTED_OBJECT_REGEX.cap(1).trimmed(), EXPECTED_OBJECT_REGEX.cap(2).toInt());

        } else if (FALSE_POSITIVE_OBJECT_REGEX.exactMatch(line)) {
            insertOrUpdate(ir.falsePositiveObjects, FALSE_POSITIVE_OBJECT_REGEX.cap(1).trimmed(), FALSE_POSITIVE_OBJECT_REGEX.cap(2).toInt());

        } else if (DETECTION_REGEX.exactMatch(line)) {
            ir.detections.append(objectFromMatch(DETECTION_REGEX));

        } else if (GROUND_TRUTH_REGEX.exactMatch(line)) {
            auto o = objectFromMatch(GROUND_TRUTH_REGEX);
            o.ground_truth = true;
            ir.groundTruth.append(o);

        } else if (ROLLING_AP_REGEX.exactMatch(line)) {
            QString k = ROLLING_AP_REGEX.cap(1);
            QVector<double> p = {
                ROLLING_AP_REGEX.cap(2).toDouble(),
                ROLLING_AP_REGEX.cap(3).toDouble(),
                ROLLING_AP_REGEX.cap(4).toDouble()
            };
            ir.rollingAP[k] = p;

        } else if (ROLLING_MEAN_AP_REGEX.exactMatch(line)) {
            ir.rollingMeanAP = ROLLING_MEAN_AP_REGEX.cap(1).toDouble();

        } else if (predictionCount > 0) {
            // parsing a prediction line
            --predictionCount;
            PredictionResult pr;
            if (PREDICTION_REGEX.exactMatch(line)) {
                pr.accuracy = PREDICTION_REGEX.cap(1).toDouble();
                pr.index = 0;
                pr.labels = PREDICTION_REGEX.cap(2).trimmed();
                pr.isCorrect = pr.labels == ir.correctLabels;
                ir.predictions.append(pr);
            } else {
                qWarning() << "Failed to parse prediction result line: " << line;
            }
        }
    }
    processPredictedResults(ir);
    emitStopped();
    if (isInterruptionRequested()) {
        qDebug() << "Worker process interrupted by user request";
        ck.waitForFinished(3 * 1000);
    } else {
        qDebug() << "Worker process finished";
    }
    qDebug() << "Closing classification data file";
    outputFile.close();

    normalExit = true;
}

void WorkerThread::emitStopped() {
    AppEvents::instance()->killChildProcesses();
    // do not actually emit anything, concerned parties must connect on QThread::finished
}

void WorkerThread::processPredictedResults(const ImageResult& imageResult) {
    if (imageResult.isEmpty()) {
        return;
    }
    while (!isInterruptionRequested() && minResultIntervalMs > QDateTime::currentMSecsSinceEpoch() - lastResultMs) {
        msleep(NORMAL_WAIT_MS);
    }
    lastResultMs = QDateTime::currentMSecsSinceEpoch();
    if (!isInterruptionRequested()) {
        emit newImageResult(imageResult);
    }
}

