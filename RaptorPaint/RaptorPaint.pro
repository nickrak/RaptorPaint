#-------------------------------------------------
#
# Project created by QtCreator 2012-02-07T19:21:34
#
#-------------------------------------------------

QT       += core gui network

TARGET = RaptorPaint
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    connectionmanager.cpp \
    connectionwindow.cpp \
    glwindow.cpp

HEADERS  += \
    netpaintinterface.h \
    mainwindow.h \
    connectionmanager.h \
    connectionwindow.h \
    glwindow.h

FORMS    += \
    mainwindow.ui \
    connectionwindow.ui

RESOURCES += \
    Resources.qrc

ICON = raptor.icns

RC_FILE = myapp.rc
