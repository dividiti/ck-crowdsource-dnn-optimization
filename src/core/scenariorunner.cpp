#include "appconfig.h"
#include "scenariorunner.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QStringList>

ScenarioRunParams::ScenarioRunParams(const CkEntry &engine, const RecognitionScenario& scenario)
{
    qDebug() << "Prepare scenario" << scenario.title();

#ifdef Q_OS_WIN32
    _program = "python";
#else
    _workdir = AppConfig::ckBinPath();
    _program = AppConfig::ckExeName();
#endif
    _arguments = QStringList()
        #ifdef Q_OS_WIN32
            << "-W"
            << "ignore::DeprecationWarning"
            << AppConfig::ckBinPath()+"\\..\\ck\\kernel.py"
        #endif
            << "run"
            << "program:" + getProgramForEngine(engine)
            << "--cmd_key=use_external_image"
            << "--deps.caffemodel=" + scenario.uid()
            << QString();
    _imageFileArgIndex = _arguments.size()-1;
    _arguments.append(AppConfig::ckArgs());

    qDebug() << "DIR:" << _workdir;
    qDebug() << "CMD:" << _program + ' ' + _arguments.join(' ');
}

QString ScenarioRunParams::getProgramForEngine(const CkEntry& engine)
{
    if (engine.title().contains("opencl"))
        return "caffe-classification-opencl";
    if (engine.title().contains("cuda"))
        return "caffe-classification-cuda";
    return "caffe-classification";
}

//-----------------------------------------------------------------------------

ScenarioRunner::ScenarioRunner(const ScenarioRunParams &params, int scenarioId, QObject *parent) : QObject(parent)
{
    _logOutput = AppConfig::logRecognitionOutput();

    _process = new QProcess(this);
    _process->setWorkingDirectory(params.workdir());
    _process->setProgram(params.program());
    connect(_process, &QProcess::errorOccurred, this, &ScenarioRunner::errorOccurred);
    if (_logOutput)
    {
        connect(_process, &QProcess::readyReadStandardOutput, this, &ScenarioRunner::readyReadStandardOutput);
        connect(_process, &QProcess::readyReadStandardError, this, &ScenarioRunner::readyReadStandardError);
    }
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));

    _arguments = params.arguments();
    _arguments << QString("--env.XOPENME_TIME_FILE=tmp-ck-timer-%1.json").arg(scenarioId);

    _imageFileArgIndex = params.imageFileArgIndex();
}

void ScenarioRunner::run(const QString& imageFile, bool waitForFinish)
{
    _error.clear();
    _stdout.clear();
    _stderr.clear();

    if (_imageFileArgIndex >= 0)
        _arguments[_imageFileArgIndex] = "--env.CK_CAFFE_PATH_TO_IMAGE=" + imageFile;
    _process->setArguments(_arguments);

    if (verboseDebugPrint)
        qDebug() << "Start command";
    qDebug() << "RUN:" << _process->program() + ' ' + _arguments.join(' ');
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

    if (_logOutput)
    {
        _stdout = _stdoutLog.join("\n");
        _stderr = _stderrLog.join("\n");
    }
    else
    {
        _stdout = QString::fromUtf8(_process->readAllStandardOutput());
        _stderr = QString::fromUtf8(_process->readAllStandardError());
    }

    if (exitStatus == QProcess::CrashExit)
        _error = _process->errorString();
    else if (exitCode != 0 || !_stderr.isEmpty())
        _error = _stderr;

    emit scenarioFinished(_error);
}

void ScenarioRunner::readyReadStandardError()
{
    auto out = _process->readAllStandardError();
    if (!out.isEmpty())
    {
        qDebug() << "STDERR"; qDebug() << out;
        _stderrLog << out;
    }
}

void ScenarioRunner::readyReadStandardOutput()
{
    auto out = _process->readAllStandardOutput();
    if (!out.isEmpty())
    {
        qDebug() << "STDOUT"; qDebug() << out;
        _stdoutLog << out;
    }
}
