#include "appevents.h"
#include "appconfig.h"
#include "ck.h"
#include "scenariosprovider.h"

#include <QDebug>

ScenariosProvider::ScenariosProvider(QObject *parent) : QObject(parent)
{
}

RecognitionScenarios ScenariosProvider::queryModelByUid(const QString& modelUid)
{
    RecognitionScenarios scenarios;
    qDebug() << "Find model by uid:" << modelUid;
    auto model = CK().queryModelByUid(modelUid);
    if (!model.isEmpty())
        scenarios.append(model);
    else
        AppEvents::error("Model not found: " + modelUid);
    return scenarios;
}

RecognitionScenarios ScenariosProvider::queryAllModels()
{
    RecognitionScenarios scenarios;
    auto models = CK().queryCaffeModels();
    for (auto model: models)
        scenarios.append(model);
    return scenarios;
}

void ScenariosProvider::setCurrentList(const RecognitionScenarios& scenarios)
{
    _current = scenarios;
}
