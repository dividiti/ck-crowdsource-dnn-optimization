#ifndef SCENARIORUNNER_H
#define SCENARIORUNNER_H

#include <QObject>
#include <QProcess>

#include "appmodels.h"

class ScenarioRunner : public QObject
{
    Q_OBJECT
public:
    explicit ScenarioRunner(QObject *parent = 0);

    void run(const RecognitionScenario& scenario, bool waitForFinish = false);

    QString readStdout() const;
    QString readStderr() const;

signals:
    void scenarioFinished(const QString &error);

private slots:
    void stateChanged(QProcess::ProcessState newState);
    void errorOccurred(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();

private:
    class ScenarioPaths
    {
    public:
        QStringList libs;
        QString exe;
        QString defaultImage;
    };

    QProcess* _process;

    ScenarioPaths processFiles(const RecognitionScenario& scenario) const;
    QString prepareProgram(const QStringList &args) const;
    QStringList prepareArguments(const QStringList &args, const QString &defaultImage) const;
    QProcessEnvironment prepareInvironment(const QStringList &libs) const;
};

#endif // SCENARIORUNNER_H
