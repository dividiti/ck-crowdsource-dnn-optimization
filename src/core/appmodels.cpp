#include "appmodels.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QFile>

void ExperimentResult::reset()
{
    imagesCount = 0;
    totalTime = 0;
    timePerImage = 0;
    imagesPerSecond = 0;
    timePerBatch = 0;
    memoryPerImage = 0;
}

void ExperimentResult::accumulate(const ExperimentProbe* p)
{
    imagesCount++;
    totalTime += p->time;
    timePerImage = totalTime/imagesCount;
    imagesPerSecond = (timePerImage > 0)? 1/timePerImage: 0;
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
    qDebug() << "Images prepared:" << _images.size();
}
