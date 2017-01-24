#include "appconfig.h"
#include "scenariorunner.h"

#include <QDebug>
#include <QStringList>

ScenarioRunner::ScenarioRunner(QObject *parent) : QObject(parent)
{
    _process = new QProcess(this);
    connect(_process, &QProcess::started, this, &ScenarioRunner::started);
    connect(_process, &QProcess::stateChanged, this, &ScenarioRunner::stateChanged);
    connect(_process, &QProcess::errorOccurred, this, &ScenarioRunner::errorOccurred);
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    connect(_process, &QProcess::readyReadStandardError, this, &ScenarioRunner::readyReadStandardError);
    connect(_process, &QProcess::readyReadStandardOutput, this, &ScenarioRunner::readyReadStandardOutput);
}

void ScenarioRunner::run(const RecognitionScenario& scenario, bool waitForFinish)
{
    qDebug() << "Run scenario" << scenario.title();
    qDebug() << "Prepare command" << scenario.cmd();

    // TODO: split more careful regarding to quoted arguments containing spaces
    QStringList args = scenario.cmd().split(' ', QString::SkipEmptyParts);

    auto paths = processFiles(scenario);

    _process->setWorkingDirectory(paths.exe);
    _process->setProgram(prepareProgram(args));
    _process->setArguments(prepareArguments(args, paths.defaultImage));
    _process->setProcessEnvironment(prepareInvironment(paths.libs));

    qDebug() << "Execute command";
    _process->start();

    if (waitForFinish)
        _process->waitForFinished();
}

ScenarioRunner::ScenarioPaths ScenarioRunner::processFiles(const RecognitionScenario& scenario) const
{
    ScenarioPaths paths;
    for (const RecognitionScenarioFileItem& file: scenario.files())
    {
        if (file.isDefaultImage())
        {
            paths.defaultImage = file.fullFileName();
            qDebug() << "IMG" << file.fullFileName();
        }
        if (file.isExecutable())
        {
            paths.libs << file.fullPath();
            qDebug() << "LIB" << file.fullPath();
            if (!file.isLibrary())
            {
                paths.exe = file.fullPath();
                qDebug() << "EXE" << file.fullPath();
            }
        }
    }
    return paths;
}

QString ScenarioRunner::prepareProgram(const QStringList &args) const
{
    if (args.isEmpty())
    {
        qCritical() << "Program name not found";
        return QString();
    }
    auto cmd = args.first();
    qDebug() << "CMD" << cmd;
    return cmd;
}

QStringList ScenarioRunner::prepareArguments(const QStringList &rawArgs, const QString& defaultImage) const
{
    const QString localPathEntry("$#local_path#$/openscience");
    const QString imageEntry("$#image#$");

    QStringList programArgs;
    for (int i = 1; i < rawArgs.size(); i++)
    {
        QString rawArg = rawArgs.at(i);
        auto arg = rawArg
                .replace(localPathEntry, AppConfig::scenariosDataDir())
                .replace(imageEntry, defaultImage);
        programArgs << arg;
        qDebug() << "ARG" << arg;
    }
    return programArgs;
}

QProcessEnvironment ScenarioRunner::prepareInvironment(const QStringList& libs) const
{
    QProcessEnvironment env;
    env.insert("CT_REPEAT_MAIN", "1");

    if (!libs.isEmpty())
    {
#ifdef Q_OS_LINUX
        env.insert("LD_LIBRARY_PATH", libs.join(":") + ":$LD_LIBRARY_PATH");
#endif
#ifdef Q_OS_MAC
        // TODO: MacOS
#endif
#ifdef Q_OS_WIN
        // TODO: Windows
#endif
    }
    return env;
}

void ScenarioRunner::stateChanged(QProcess::ProcessState newState)
{
    qDebug() << "ScenarioRunner::stateChanged()" << newState;
}

void ScenarioRunner::errorOccurred(QProcess::ProcessError error)
{
    qDebug() << "ScenarioRunner::errorOccurred()" << error << _process->errorString();
    emit scenarioFinished(_process->errorString());
}

void ScenarioRunner::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "ScenarioRunner::finished()" << exitCode << exitStatus;

    if (exitStatus == QProcess::CrashExit)
        emit scenarioFinished(_process->errorString());
    else
        emit scenarioFinished(QString());
}

void ScenarioRunner::readyReadStandardError()
{
    qDebug() << "ScenarioRunner::readyReadStandardError()";
}

void ScenarioRunner::readyReadStandardOutput()
{
    qDebug() << "ScenarioRunner::readyReadStandardOutput()";
}

void ScenarioRunner::started()
{
    qDebug() << "ScenarioRunner::started()";
}

QString ScenarioRunner::readStdout() const
{
    return QString::fromUtf8(_process->readAllStandardOutput());
}

QString ScenarioRunner::readStderr() const
{
    return QString::fromUtf8(_process->readAllStandardError());
}
