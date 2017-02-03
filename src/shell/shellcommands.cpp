#include "appconfig.h"
#include "ck.h"
#include "recognizer.h"
#include "shellcommands.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>

ShellCommands::ShellCommands(QObject *parent) : QObject(parent)
{
}

ShellCommands::Result ShellCommands::process(const QApplication &app)
{
    QCommandLineParser cmdLine;
    cmdLine.addHelpOption();
    cmdLine.addVersionOption();

    QCommandLineOption option_caffeModels("1", "Enumerate caffe models.");
    cmdLine.addOption(option_caffeModels);

    QCommandLineOption option_engines("2", "Enumerate caffe libs.");
    cmdLine.addOption(option_engines);

    QCommandLineOption option_imageSources("3", "Enumerate image datasets.");
    cmdLine.addOption(option_imageSources);

    QCommandLineOption option_runRecognition("4", "Run recognition with specified model.");
    cmdLine.addOption(option_runRecognition);

    QCommandLineOption option_editStyle("5", "Edit application style.");
    cmdLine.addOption(option_editStyle);

    QCommandLineOption option_recognitionEngine("engine", QString("caffe-lib uid to run recognition (for command %1).")
                                               .arg(option_runRecognition.names().first()), "uid");
    cmdLine.addOption(option_recognitionEngine);

    QCommandLineOption option_recognitionModel("model", QString("caffe-model uid to run recognition (for command %1).")
                                               .arg(option_runRecognition.names().first()), "uid");
    cmdLine.addOption(option_recognitionModel);

    QCommandLineOption option_recognitionImages("images", QString("imagenet dataset uid to run recognition (for command %1).")
                                               .arg(option_runRecognition.names().first()), "uid");
    cmdLine.addOption(option_recognitionImages);

    QCommandLineOption option_recognize("6", "Recognize smth.");
    cmdLine.addOption(option_recognize);


    cmdLine.process(app);

    if (cmdLine.isSet(option_caffeModels))
    {
        command_caffeModels();
        return CommandFinished;
    }

    if (cmdLine.isSet(option_engines))
    {
        command_engines();
        return CommandFinished;
    }

    if (cmdLine.isSet(option_imageSources))
    {
        command_imageSources();
        return CommandFinished;
    }

    if (cmdLine.isSet(option_editStyle))
    {
        _appParams.runMode = AppRunParams::EditStyle;
        return ParamsAcquired;
    }

    if (cmdLine.isSet(option_runRecognition))
    {
        _appParams.engineUid = cmdLine.value(option_recognitionEngine);
        _appParams.modelUid = cmdLine.value(option_recognitionModel);
        _appParams.imagesUid = cmdLine.value(option_recognitionImages);
        _appParams.startImmediately = true;
        return ParamsAcquired;
    }

    if (cmdLine.isSet(option_recognize))
    {
        command_recognize();
        return CommandFinished;
    }

    return CommandIgnored;
}

QTextStream& ShellCommands::cout()
{
    static QTextStream s(stdout);
    return s;
}

void ShellCommands::command_engines()
{
    for (auto lib: CK().queryEngines())
        cout() << lib.str() << endl;
}

void ShellCommands::command_caffeModels()
{
    for (auto model: CK().queryCaffeModels())
        cout() << model.str() << endl;
}

void ShellCommands::command_imageSources()
{
    for (auto dataset: CK().queryCaffeImages())
        cout() << dataset.str() << endl;
}

void ShellCommands::command_recognize()
{
    QString lib("/home/nikolay/CK-TOOLS/dnn-proxy-caffe-0.1-gcc-5.4.0-linux-64/lib/libdnnproxy.so");
    QString model("/home/nikolay/CK/ck-caffe/program/caffe-classification/tmp/tmp-05LaUH.prototxt");
    QString weights("/home/nikolay/CK-TOOLS/caffemodel-bvlc-googlenet/bvlc_googlenet.caffemodel");
    QString mean("/home/nikolay/CK/ck-caffe/program/caffe-classification/imagenet_mean.binaryproto");
    QString labels("/home/nikolay/CK/ck-caffe/program/caffe-classification/synset_words.txt");
    QString image("/home/nikolay/Projects/crowdsource-video-experiments-on-desktop/images/sample1.jpg");
    Recognizer r(lib);
    if (!r.ready()) return;
    r.prepare(model, weights, mean, labels);
    ExperimentProbe probe;
    r.recognize(image, probe);
    cout() << "time: " << QString::number(probe.time) << endl;
    cout() << "memory: " << QString::number(probe.memory) << endl;
    for (auto p: probe.predictions)
        cout() << QString("%1 - %3 - %2").arg(p.accuracy).arg(p.description).arg(p.index) << endl;
}

