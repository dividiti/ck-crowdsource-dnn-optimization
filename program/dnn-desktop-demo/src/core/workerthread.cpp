#include "workerthread.h"
#include "appconfig.h"
#include "appevents.h"

#include <QProcess>
#include <QTextStream>
#include <QRegExp>
#include <QDebug>
#include <QFile>

static QString getExe() {
#ifdef Q_OS_WIN32
    return "python";
#else
    QString ckExe = AppConfig::ckExeName();
    if (ckExe.isEmpty()) {
        AppEvents::error("CK bin path not found in config");
    }
    return ckExe;
#endif
}

static QStringList getDefaultArgs() {
#ifdef Q_OS_WIN32
    return QStringList { "-W", "ignore::DeprecationWarning", AppConfig::ckBinPath() + "\\..\\ck\\kernel.py" };
#else
    return QStringList();
#endif
}

static QString getBinPath() {
    auto ckDir = AppConfig::ckBinPath();
    if (ckDir.isEmpty()) {
        AppEvents::error("CK exe name not found in config");
    }
    return ckDir;
}

static const QString FILE_PREFIX = "File: ";
static const QString DURATION_PREFIX = "Duration: ";
static const QString DURATION_SUFFIX = " sec";
static const QString CORRECT_LABEL_PREFIX = "Correct label: ";
static const QString PREDICTION_PREFIX = "Predictions: ";
static const QRegExp PREDICTION_REGEX("([0-9]*\\.?[0-9]+) - \"([^\"]+)\"");

WorkerThread::WorkerThread(const Program& program, const Model& model, const Dataset& dataset, QObject* parent)
    : QThread(parent), program(program), model(model), dataset(dataset) {}

void WorkerThread::run() {
    QProcess ck;
    ck.setWorkingDirectory(getBinPath());
    ck.setProgram(getExe());
    QStringList fullArgs = getDefaultArgs();
    auto args = QStringList {
            "run",
            "program:" + program.uoa,
            "--cmd_key=use_continuous",
            "--deps.caffemodel=" + model.uoa,
            "--deps.imagenet-aux=" + dataset.auxUoa,
            "--deps.imagenet-val=" + dataset.valUoa
            };
    fullArgs.append(args);
    ck.setArguments(fullArgs);

    qDebug() << "Run CK command:" << ck.program() + " " +  ck.arguments().join(" ");

    QFile outputFile(program.outputFile);
    outputFile.remove();

    ck.start();

    while (!outputFile.exists() && !isInterruptionRequested()) {
        msleep(100);
    }

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
            finished = ck.waitForFinished(50);
            continue;
        }
        line = line.trimmed();
        if (line.isEmpty()) {
            processPredictedResults(ir);
            ir = ImageResult();

        } else if (line.startsWith(FILE_PREFIX)) {
            ir.imageFile = line.mid(FILE_PREFIX.size());

        } else if (line.startsWith(DURATION_PREFIX)) {
            QStringRef t = line.midRef(DURATION_PREFIX.size());
            t = t.left(t.size() - DURATION_SUFFIX.size());
            ir.duration = t.toDouble();

        } else if (line.startsWith(CORRECT_LABEL_PREFIX)) {
            ir.correctLabels = line.mid(CORRECT_LABEL_PREFIX.size()).trimmed();

        } else if (line.startsWith(PREDICTION_PREFIX)) {
            predictionCount = line.mid(PREDICTION_PREFIX.size()).toInt();

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
    if (isInterruptionRequested()) {
        qDebug() << "Worker process interrupted by user request";
        ck.kill();
        ck.waitForFinished();
    } else {
        qDebug() << "Worker process finished";
    }
    outputFile.close();
    AppEvents::instance()->killChildProcesses();
    emit stopped();
}

void WorkerThread::processPredictedResults(const ImageResult& imageResult) {
    if (imageResult.isEmpty()) {
        return;
    }
    emit newImageResult(imageResult);
}

