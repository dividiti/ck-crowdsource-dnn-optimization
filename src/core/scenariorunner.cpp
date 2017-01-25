#include "appconfig.h"
#include "scenariorunner.h"

#include <QDebug>
#include <QStringList>

ScenarioRunParams::ScenarioRunParams(const RecognitionScenario& scenario)
{
    qDebug() << "Prepare scenario" << scenario.title() << scenario.cmd();

    // TODO: split more careful regarding to quoted arguments containing spaces
    QStringList args = scenario.cmd().split(' ', QString::SkipEmptyParts);

    processFiles(scenario);
    prepareProgram(args);
    prepareArguments(args);
    prepareInvironment();
}

void ScenarioRunParams::processFiles(const RecognitionScenario& scenario)
{
    for (const RecognitionScenarioFileItem& file: scenario.files())
    {
        if (file.isExecutable())
        {
            auto path = file.fullPath();
            _paths << path;
            qDebug() << "LIB" << path;

            if (!file.isLibrary())
            {
                _workdir = path;
                qDebug() << "EXE" << path;
            }
        }
    }
}

void ScenarioRunParams::prepareProgram(const QStringList &args)
{
    if (args.isEmpty())
    {
        qCritical() << "Program name not found";
        return;
    }
    _program = args.first();
    qDebug() << "CMD" << _program;
}

void ScenarioRunParams::prepareArguments(const QStringList &args)
{
    static QString localPathEntry("$#local_path#$/openscience");
    static QString imageEntry("$#image#$");

    const QString scenarioDataDir = AppConfig::scenariosDataDir();

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
        qDebug() << "ARG" << (arg.isEmpty()? QString("<image path here>"): arg);
}

void ScenarioRunParams::prepareInvironment()
{
    _env.insert("CT_REPEAT_MAIN", "1");

    if (!_paths.isEmpty())
    {
#ifdef Q_OS_LINUX
        _env.insert("LD_LIBRARY_PATH", _paths.join(":") + ":$LD_LIBRARY_PATH");
#endif
#ifdef Q_OS_MAC
        _env.insert("DYLD_FALLBACK_LIBRARY_PATH", _paths.join(":") + ":$DYLD_FALLBACK_LIBRARY_PATH");
    #endif
#ifdef Q_OS_WIN
        // TODO: Windows
#endif
    }
}

//-----------------------------------------------------------------------------

ScenarioRunner::ScenarioRunner(const ScenarioRunParams &params, QObject *parent) : QObject(parent)
{
    _process = new QProcess(this);
    _process->setWorkingDirectory(params.workdir());
    _process->setProgram(params.program());
    _process->setProcessEnvironment(params.environment());
    connect(_process, &QProcess::errorOccurred, this, &ScenarioRunner::errorOccurred);
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));

    _arguments = params.arguments();
    _imageFileArgIndex = params.imageFileArgIndex();
}

void ScenarioRunner::run(const QString& imageFile, bool waitForFinish)
{
    _error.clear();
    _stdout.clear();
    _stderr.clear();

    if (_imageFileArgIndex >= 0)
        _arguments[_imageFileArgIndex] = imageFile;
    _process->setArguments(_arguments);

    if (verboseDebugPrint)
        qDebug() << "Start command";
    _process->start();

    if (waitForFinish)
        _process->waitForFinished();
}

void ScenarioRunner::errorOccurred(QProcess::ProcessError error)
{
    qDebug() << "ScenarioRunner::errorOccurred()" << error << _process->errorString();

    emit scenarioFinished(_process->errorString());
}

void ScenarioRunner::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (verboseDebugPrint)
        qDebug() << "ScenarioRunner::finished()" << exitCode << exitStatus;

    _stdout = QString::fromUtf8(_process->readAllStandardOutput());
    _stderr = QString::fromUtf8(_process->readAllStandardError());

    if (exitStatus == QProcess::CrashExit)
        _error = _process->errorString();
    else if (exitCode != 0 || !_stderr.isEmpty())
        _error = _stderr;

    emit scenarioFinished(_error);
}

