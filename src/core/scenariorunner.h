#ifndef SCENARIORUNNER_H
#define SCENARIORUNNER_H

#include <QObject>
#include <QProcess>

#include "appmodels.h"

class ScenarioRunParams
{
public:
    ScenarioRunParams(const CkEntry& engine, const RecognitionScenario& scenario);

    QString program() const { return _program; }
    QString workdir() const { return _workdir; }
    const QStringList& arguments() const { return _arguments; }
    int imageFileArgIndex() const { return _imageFileArgIndex; }

private:
    QStringList _arguments;
    QString _workdir, _program;
    int _imageFileArgIndex = -1;

    QString getProgramForEngine(const CkEntry& engine);
};

//-----------------------------------------------------------------------------

class ScenarioRunner : public QObject
{
    Q_OBJECT
public:
    explicit ScenarioRunner(const ScenarioRunParams &params, int scenarioId, QObject *parent = 0);

    void run(const QString& imageFile, bool waitForFinish = false);

    QString getStdout() const { return _stdout; }
    QString getStderr() const { return _stderr; }

    bool verboseDebugPrint = false;

    bool ok() const { return _error.isEmpty(); }
    QString error() const { return _error; }

signals:
    void scenarioFinished(const QString &error);

private slots:
    void errorOccurred(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();

private:
    QProcess* _process;
    QStringList _arguments;
    int _imageFileArgIndex;
    QString _error, _stderr, _stdout;
    QStringList _stdoutLog, _stderrLog;
    bool _logOutput;
};

#endif // SCENARIORUNNER_H
