#include "appmodels.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QFile>

QString PredictionResult::str() const
{
    return QString(QStringLiteral("%1 - %2 %3"))
        .arg(accuracy).arg(labels)
        .arg(isCorrect ? QString(QStringLiteral(" CORRECT")) : QString());
}

//-----------------------------------------------------------------------------

void ExperimentResult::reset()
{
    imagesCount = 0;
    totalTime = 0;
    timePerImage = 0;
    imagesPerSecond = 0;
    timePerBatch = 0;
    memoryPerImage = 0;

    top1Metric = 0;
    top5Metric = 0;
    top1Count = 0;
    top5Count = 0;

    worstPredictedImage.clear();
    worstPredictionMarker = 0;
    worstPredictionFlag = false;
}

void ExperimentResult::accumulate(const ExperimentProbe* p)
{
    imagesCount++;
    totalTime += p->time;
    timePerImage = totalTime/imagesCount;
    imagesPerSecond = (timePerImage > 0)? 1/timePerImage: 0;

    if (p->correctAsTop1)
    {
        top1Count++;
        top1Metric = top1Count / double(imagesCount);
    }
    if (p->correctAsTop5)
    {
        top5Count++;
        top5Metric = top5Count / double(imagesCount);
    }

    calculateWorstPrediction(p);
}

void ExperimentResult::calculateWorstPrediction(const ExperimentProbe *p)
{
    if (!p->correctAsTop1)
    {
        auto correctToHighest = p->predictions.at(0).accuracy - p->correctInfo.accuracy;
        worstPredictionFlag = correctToHighest >= worstPredictionMarker;
        if (worstPredictionFlag)
        {
            worstPredictionMarker = correctToHighest;
            worstPredictedImage = p->image;
            worstPredictionCorrect = p->correctInfo;
            worstPredictionTop1 = p->predictions.at(0);

            qDebug() << "----------------------------------------------";
            qDebug() << "Worst prediction found with marker value" << worstPredictionMarker;
            qDebug() << "Image:" << worstPredictedImage;
            qDebug() << "Top-1:" << worstPredictionTop1.str();
            qDebug() << "Correct:" << worstPredictionCorrect.str();
        }
    }
    else worstPredictionFlag = false;
}

//-----------------------------------------------------------------------------

ImagesBank::ImagesBank(const QString& imagesDir, const QString& valFile)
{
    qDebug() << "Prepare images from" << valFile << imagesDir;
    auto vals = QString::fromUtf8(Utils::loadTextFromFile(valFile));
    auto lines = vals.splitRef('\n', QString::SkipEmptyParts);
    _images.reserve(lines.size());
    for (const QStringRef& line: lines)
    {
        int pos = line.lastIndexOf(' ');
        if (pos < 1)
        {
            qWarning() << "No correctness index in line" << line.toString();
            continue;
        }
        bool ok;
        int index = line.right(line.length()-pos-1).toInt(&ok);
        if (!ok)
        {
            qWarning() << "Invalid correctness index in line" << line.toString();
            continue;
        }
        auto fileName = imagesDir + QDir::separator() + line.left(pos).toString();
        if (!QFile(fileName).exists())
        {
            qWarning() << "File not found for line" << line.toString();
            continue;
        }
        _images << ImageEntry { fileName, index };
    }
    qDebug() << "Images prepared: " << _images.size();
}
