#ifndef EXPERIMENTCONTEXT_H
#define EXPERIMENTCONTEXT_H

#include "appmodels.h"

#include <QObject>
#include <QList>

template<typename TItem> class ListContainer
{
public:
    bool hasCurrent() const { return checkIndex(_currenIndex); }
    const TItem& current() const { return _items.at(_currenIndex); }

    int currentIndex() const { return _currenIndex; }
    void setCurrentIndex(int index) { if (checkIndex(index)) _currenIndex = index; }
    void setCurrentIndexOrDefault(int index);
    bool checkIndex(int index) const { return index >= 0 && index < _items.size(); }

    const QList<TItem>& items() const { return _items; }
    bool isEmpty() const { return _items.isEmpty(); }

    bool selectCurrentViaDialog();

private:
    int _currenIndex = -1;
    QList<TItem> _items;

    friend class MainWindow;
};

//-----------------------------------------------------------------------------

class ExperimentContext : public QObject
{
    Q_OBJECT

public:
    int experimentIndex() const { return _experimentIndex; }

    ListContainer<DnnModel>& models() { return _models; }
    ListContainer<DnnEngine>& engines() { return _engines; }
    ListContainer<ImagesDataset>& images() { return _images; }

    void startExperiment();
    void stopExperiment();
    bool isExperimentStarted() const { return _isExperimentStarted; }
    const ExperimentResult& experimentResult() const { return _result; }

    void loadFromConfig();

signals:
    void experimentStarted();
    void experimentStopping();
    void experimentFinished();
    void experimentResultReady();

public slots:
    void recognitionFinished(const ExperimentProbe* p);

private:
    int _experimentIndex = -1;
    bool _isExperimentStarted = false;
    ExperimentResult _result;
    ListContainer<DnnModel> _models;
    ListContainer<DnnEngine> _engines;
    ListContainer<ImagesDataset> _images;

    friend class MainWindow;
};

#endif // EXPERIMENTCONTEXT_H
