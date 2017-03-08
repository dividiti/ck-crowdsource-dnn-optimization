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
