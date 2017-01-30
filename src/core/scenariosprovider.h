#ifndef SCENARIOSPROVIDER_H
#define SCENARIOSPROVIDER_H

#include <QObject>

#include "appmodels.h"

class ScenariosProvider : public QObject
{
    Q_OBJECT

public:
    explicit ScenariosProvider(QObject *parent = 0);

    static RecognitionScenarios queryFromCK();

    const RecognitionScenarios& currentList() const { return _current; }
    void setCurrentList(const RecognitionScenarios& scenarios);

signals:
    void scenariosReceived(RecognitionScenarios scenarios);

private:
    RecognitionScenarios _current;
};

#endif // SCENARIOSPROVIDER_H
