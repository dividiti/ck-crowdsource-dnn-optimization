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
