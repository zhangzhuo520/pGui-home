#-------------------------------------------------
#
# Project created by QtCreator 2019-01-15T04:07:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LogQt
TEMPLATE = app

include(log4qt/log4qt.pri)

SOURCES += main.cpp\
    qtlogger.cc \
    pgui_log_global.cc

HEADERS  +=  qtlogger.h \
    pgui_log_global.h

RESOURCES += \
    rc.qrc
