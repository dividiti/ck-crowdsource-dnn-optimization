#include "appmodels.h"

QString CkEntry::str() const
{
    return QString("%1: %2").arg(uid, name);
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
}

void ExperimentResult::accumulate(const ExperimentProbe& p)
{
    imagesCount++;
    totalTime += p.time;
    timePerImage = totalTime/imagesCount;
    imagesPerSecond = (timePerImage > 0)? 1/timePerImage: 0;
}

//-----------------------------------------------------------------------------

QString ImagesDataset::str() const
{
    return _title;
}

void ImagesDataset::buildCorrectnessMap()
{
    // TODO
}

//-----------------------------------------------------------------------------

QString DnnEngine::str() const
{
    return QString("%1: %2").arg(_title, _libFile);
}
