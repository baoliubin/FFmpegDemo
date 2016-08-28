#-------------------------------------------------
#
# Project created by QtCreator 2016-08-20T08:32:26
#
#-------------------------------------------------

QT       += core gui qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Capture
TEMPLATE = app

FFMPEGPATH = E:/FFMPEG/ffmpeg-dev
INCLUDEPATH += $${FFMPEGPATH}/include
DEPENDPATH += $${FFMPEGPATH}/include

LIBS += -L$${FFMPEGPATH}/lib -lavcodec \
    -lavdevice \
    -lavfilter \
    -lavformat \
    -lavutil \
    -lpostproc \
    -lswresample \
    -lswscale

SOURCES += main.cpp\
        mainwindow.cpp \
    videodec.cpp

HEADERS  += mainwindow.h \
    videodec.h

FORMS    += mainwindow.ui

RESOURCES += \
    qml.qrc
