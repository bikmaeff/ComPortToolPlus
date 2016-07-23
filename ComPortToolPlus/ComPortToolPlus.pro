#-------------------------------------------------
#
# Project created by QtCreator 2015-09-03T17:49:55
# when you add a new lib to this file, Build->Excute Qmake must be done!
#
#-------------------------------------------------

QT       += core gui widgets serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



TARGET = ComPortToolPlus
TEMPLATE = app


SOURCES += main.cpp\
    wavewindow.cpp \
    qcustomplot.cpp \
    receivethread.cpp \
    sendthread.cpp \
    mainwindow.cpp \
    aboutwindow.cpp

HEADERS  += mainwindow.h \
    wavewindow.h \
    qcustomplot.h \
    receivethread.h \
    sendthread.h \
    aboutwindow.h

FORMS    += mainwindow.ui \
    wavewindow.ui \
    aboutwindow.ui

DISTFILES += \
    appIcon.rc \

RC_FILE = \
   appIcon.rc
TRANSLATIONS += en.ts \
    cn_zh.ts

RESOURCES += \
    resources.qrc
