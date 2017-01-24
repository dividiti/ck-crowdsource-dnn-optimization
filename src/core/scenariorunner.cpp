#include "appconfig.h"
#include "scenariorunner.h"

#include <QDebug>
#include <QStringList>

ScenarioRunner::ScenarioRunner(QObject *parent) : QObject(parent)
{
    _process = new QProcess(this);
    //connect(_process, &QProcess::started, this, &ScenarioRunner::started);
    //connect(_process, &QProcess::stateChanged, this, &ScenarioRunner::stateChanged);
    connect(_process, &QProcess::errorOccurred, this, &ScenarioRunner::errorOccurred);
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    //connect(_process, &QProcess::readyReadStandardError, this, &ScenarioRunner::readyReadStandardError);
    //connect(_process, &QProcess::readyReadStandardOutput, this, &ScenarioRunner::readyReadStandardOutput);
}

void ScenarioRunner::prepare(const RecognitionScenario& scenario)
{
    qDebug() << "Prepare scenario" << scenario.title() << scenario.cmd();

    // TODO: split more careful regarding to quoted arguments containing spaces
    QStringList args = scenario.cmd().split(' ', QString::SkipEmptyParts);

    auto paths = processFiles(scenario);

    _process->setWorkingDirectory(paths.exe);
    _process->setProgram(prepareProgram(args));
    _process->setProcessEnvironment(prepareInvironment(paths.libs));
    prepareArguments(args);
}

void ScenarioRunner::run(const QString& imageFile, bool waitForFinish)
{
    if (_imageFileArgIndex >= 0)
        _arguments[_imageFileArgIndex] = imageFile;
    _process->setArguments(_arguments);

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
            auto path = file.fullPath();
            paths.libs << path;
            qDebug() << "LIB" << path;
            if (!file.isLibrary())
            {
                paths.exe = path;
                qDebug() << "EXE" << path;
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

void ScenarioRunner::prepareArguments(const QStringList &args)
{
    const QString localPathEntry("$#local_path#$/openscience");
    const QString imageEntry("$#image#$");

    const QString scenarioDataDir = AppConfig::scenariosDataDir();

    _arguments.clear();
    _imageFileArgIndex = -1;
    for (int i = 1; i < args.size(); i++)
    {
        QString arg = args.at(i);
        if (arg.contains(imageEntry))
        {
            _imageFileArgIndex = i-1;
            _arguments << QString();
        }
        else
            _arguments << arg.replace(localPathEntry, scenarioDataDir);
    }
    for (const QString& arg: _arguments)
        qDebug() << "ARG" << (arg.isEmpty()? QString("<path to image here>"): arg);
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
