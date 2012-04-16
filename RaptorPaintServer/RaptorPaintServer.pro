#-------------------------------------------------
#
# Project created by QtCreator 2012-02-19T12:06:27
#
#-------------------------------------------------

QT       += core gui network

TARGET = RaptorPaintServer
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    sockethandler.cpp \
bufferedtube.cpp

HEADERS  += mainwindow.h \
    sockethandler.h \
bufferedtube.h

FORMS    += mainwindow.ui
