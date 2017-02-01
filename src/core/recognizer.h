#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QString>
#include <QVector>

QT_BEGIN_NAMESPACE
class QLibrary;
QT_END_NAMESPACE

#include "appmodels.h"

class Recognizer
{
public:
    Recognizer(const QString &proxyLib,
               const QString& modelFile,
               const QString& weightFile,
               const QString& meanFile,
               const QString& labelFile);
    ~Recognizer();

    QVector<PredictionResult> recognize(const QString& imageFile);

    bool ready() const;

private:
    QLibrary* _lib = nullptr;
};

#endif // RECOGNIZER_H
