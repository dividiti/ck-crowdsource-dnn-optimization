#-------------------------------------------------
#
# Project created by QtCreator 2017-01-17T12:23:57
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = crowdsource-video-experiments-on-desktop
TEMPLATE = app
DESTDIR = $$_PRO_FILE_PWD_/bin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$_PRO_FILE_PWD_/src/core

SOURCES += \
    src/main.cpp\
    src/gui/mainwindow.cpp \
    src/gui/experimentpanel.cpp \
    src/gui/framespanel.cpp \
    src/gui/featurespanel.cpp \
    src/gui/resultspanel.cpp \
    src/gui/framewidget.cpp \
    src/core/appconfig.cpp \
    src/core/appevents.cpp \
    src/ori/OriWidgets.cpp \
    src/core/experimentcontext.cpp \
    src/gui/imageview.cpp \
    src/core/workerthread.cpp \
    src/gui/recognitionwidget.cpp \
    src/gui/footerpanel.cpp \
    src/ori/clickablelabel.cpp

HEADERS += \
    src/gui/mainwindow.h \
    src/gui/experimentpanel.h \
    src/gui/framespanel.h \
    src/gui/featurespanel.h \
    src/gui/resultspanel.h \
    src/gui/framewidget.h \
    src/core/appconfig.h \
    src/core/appevents.h \
    src/core/appmodels.h \
    src/ori/OriWidgets.h \
    src/core/experimentcontext.h \
    src/gui/imageview.h \
    src/core/workerthread.h \
    src/gui/recognitionwidget.h \
    src/gui/footerpanel.h \
    src/ori/clickablelabel.h

RESOURCES += \
    src/resources.qrc
