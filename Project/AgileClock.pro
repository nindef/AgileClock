#-------------------------------------------------
#
# Project created by QtCreator 2016-09-30T16:47:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += multimedia

TARGET = AgileClock
TEMPLATE = app


SOURCES += main.cpp \
    maindialog.cpp \
    mainframe.cpp

HEADERS  += \
    maindialog.h \
    mainframe.h

RESOURCES += \
    resources.qrc

win32:RC_ICONS += appicon.ico