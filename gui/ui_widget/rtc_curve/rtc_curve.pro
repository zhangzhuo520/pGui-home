#-------------------------------------------------
#
# Project created by QtCreator 2019-03-04T01:08:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rtc_curve
TEMPLATE = app


SOURCES += main.cpp \
    ui_rtscurve.cc \
    ui_curve_widget.cc \
    qcustomplot.cc \
    curvetracer.cc

HEADERS  += \
    ui_rtscurve.h \
    ui_curve_widget.h \
    qcustomplot.h \
    curvetracer.h

RESOURCES += \
    images.qrc \
    images.qrc

