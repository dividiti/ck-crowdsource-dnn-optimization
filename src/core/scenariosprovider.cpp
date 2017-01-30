#include "appconfig.h"
#include "ck.h"
#include "scenariosprovider.h"

#include <QDebug>

ScenariosProvider::ScenariosProvider(QObject *parent) : QObject(parent)
{
}

RecognitionScenarios ScenariosProvider::queryFromCK()
{
    RecognitionScenarios scenarios;
    scenarios.loadFromCK(CK().queryCaffeModels());
    return scenarios;
}

void ScenariosProvider::setCurrentList(const RecognitionScenarios& scenarios)
{
    _current = scenarios;
}
