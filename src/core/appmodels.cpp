#include "appconfig.h"
#include "appevents.h"
#include "appmodels.h"
#include "jsonformat.h"
#include "utils.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

QString CkEntry::str() const
{
    return QString("%1: %2").arg(uid, name);
}

//-----------------------------------------------------------------------------

void RecognitionScenario::parseCK(const CkEntry& entry)
{
    _uid = entry.uid;
    _title = entry.name;
}

QString RecognitionScenario::str() const
{
    return QString("%1: %2").arg(_uid, _title);
}

QString RecognitionScenario::html() const
{
    return QString("%1: <b>%2</b>").arg(_uid, _title);
}

//-----------------------------------------------------------------------------

void RecognitionScenarios::append(const CkEntry& entry)
{
    RecognitionScenario scenario;
    scenario.parseCK(entry);
    _items << scenario;
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
