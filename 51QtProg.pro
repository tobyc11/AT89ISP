#-------------------------------------------------
#
# Project created by QtCreator 2016-10-14T21:57:29
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 51QtProg
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    intelhexclass.cpp

HEADERS  += MainWindow.h \
    intelhexclass.h

FORMS    += MainWindow.ui

LIBS += E:/Dev/qhexedit2/build/Release/qhexedit2.lib
INCLUDEPATH += E:/Dev/qhexedit2/src
