#ifndef SCENARIORUNNER_H
#define SCENARIORUNNER_H

#include <QObject>
#include <QProcess>

#include "appmodels.h"

class ScenarioRunParams
{
public:
    ScenarioRunParams(const RecognitionScenario& scenario);

    QString program() const { return _program; }
    QString workdir() const { return _workdir; }
    const QStringList& arguments() const { return _arguments; }
    int imageFileArgIndex() const { return _imageFileArgIndex; }
    const QProcessEnvironment& environment() const { return _env; }

private:
    QProcessEnvironment _env;
    QStringList _paths, _arguments;
    QString _workdir, _program;
    int _imageFileArgIndex = -1;

    void processFiles(const RecognitionScenario& scenario);
    void prepareProgram(const QStringList &args);
    void prepareArguments(const QStringList &args);
    void prepareInvironment();
};

//-----------------------------------------------------------------------------

class ScenarioRunner : public QObject
{
    Q_OBJECT
public:
    explicit ScenarioRunner(const ScenarioRunParams &params, QObject *parent = 0);

    void run(const QString& imageFile, bool waitForFinish = false);

    QString stdout() const { return _stdout; }
    QString stderr() const { return _stderr; }

    bool verboseDebugPrint = false;

    bool ok() const { return _error.isEmpty(); }
    QString error() const { return _error; }

    ExperimentProbe readProbe() const;

signals:
    void scenarioFinished(const QString &error);

private slots:
    void errorOccurred(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess* _process;
    QStringList _arguments;
    int _imageFileArgIndex;
    QString _error, _stderr, _stdout;
    QString _timersFile;
};

#endif // SCENARIORUNNER_H
