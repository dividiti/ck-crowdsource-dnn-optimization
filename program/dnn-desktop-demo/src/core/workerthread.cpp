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
    return QStringList { "-W", "ignore::DeprecationWarning", ckDir + "\\..\\ck\\kernel.py" };
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

void WorkerThread::run() {
    QProcess ck;
    ck.setWorkingDirectory(getBinPath());
    ck.setProgram(getExe());
    QStringList fullArgs = getDefaultArgs();
    auto args = QStringList { "run", "program:caffe-classification", "--cmd_key=use_continuous", "--deps.caffemodel=6b8dde7134ceb818" };
    fullArgs.append(args);
    ck.setArguments(fullArgs);

    qDebug() << "Run CK command:" << ck.program() << " " <<  ck.arguments().join(" ");

    QFile outputFile("/home/dsavenko/CK/ck-caffe/program/caffe-classification/tmp/tmp-output1.tmp");
    outputFile.remove();

    ck.start();

    const QString fileLinePrefix = "File: ";
    const QString durationLinePrefix = "Duration: ";
    const QString durationLineSuffix = " sec";
    const QString correctLabelLinePrefix = "Correct label: ";
    const QString predictionsLinePrefix = "Predictions: ";
    const QRegExp predictionRegExp("([0-9]*\\.?[0-9]+) - \"([^\"]+)\"");

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

        } else if (line.startsWith(fileLinePrefix)) {
            ir.imageFile = line.mid(fileLinePrefix.size());

        } else if (line.startsWith(durationLinePrefix)) {
            QStringRef t = line.midRef(durationLinePrefix.size());
            t = t.left(t.size() - durationLineSuffix.size());
            ir.duration = t.toDouble();

        } else if (line.startsWith(correctLabelLinePrefix)) {
            ir.correctLabels = line.mid(correctLabelLinePrefix.size()).trimmed();

        } else if (line.startsWith(predictionsLinePrefix)) {
            predictionCount = line.mid(predictionsLinePrefix.size()).toInt();

        } else if (predictionCount > 0) {
            // parsing a prediction line
            --predictionCount;
            PredictionResult pr;
            if (predictionRegExp.exactMatch(line)) {
                pr.accuracy = predictionRegExp.cap(1).toDouble();
                pr.index = 0;
                pr.labels = predictionRegExp.cap(2).trimmed();
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
        ck.terminate();
        ck.waitForFinished();
    } else {
        qDebug() << "Worker process finished";
    }
    outputFile.close();
    emit stopped();
}

void WorkerThread::processPredictedResults(const ImageResult& imageResult) {
    if (imageResult.isEmpty()) {
        return;
    }
    emit newImageResult(imageResult);
}

