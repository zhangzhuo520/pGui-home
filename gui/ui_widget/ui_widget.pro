#-------------------------------------------------
#
# Project created by QtCreator 2018-09-19T18:06:30
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PanGenGUI
TEMPLATE = app

include(db/db.pri)
include(model/model.pri)
include(painter/painter.pri)
include(deftools/deftools.pri)
include(log4qt/log4qt.pri)
include(fileparsing/file_parsing.pri)
include(file_generate/file_generate.pri)
include(rts_setup/rts_setup.pri)

SOURCES += main.cc\
    ui_mainwindow.cc \
    ui_checklist.cc \
    ui_defgroup.cc \
    ui_defects_widget.cc \
    ui_layer_widget.cc \
    ui_fileproject_widget.cc \
    ui_frame.cc \
    ui_menu_widget.cc \
    ui_choosefile_dialog.cc \
    ui_semimage_dialog.cc \
    ui_chipedit_dialog.cc \
    ui_scale_frame.cc \
    ui_measurepoint.cc \
    ui_paint_widget.cc \
    ui_paint_toolbar.cc \
    ui_tab_paintwidget.cc \
    ui_measure_table.cc \
    ui_gaugetable.cc


HEADERS  += mainwindow.h \
    ui_checklist.h \
    ui_defgroup.h \
    ui_defects_widget.h \
    ui_fileproject_widget.h \
    ui_layer_widget.h \
    ui_frame.h \
    ui_menu_widget.h \
    ui_choosefile_dialog.h \
    ui_semimage_dialog.h \
    ui_chipedit_dialog.h \
    ui_scale_frame.h \
    ui_measurepoint.h \
    ui_paint_widget.h \
    ui_paint_toolbar.h \
    ui_tab_paintwidget.h \
    ui_measure_table.h \
    ui_gaugetable.h \

FORMS    +=

RESOURCES += \
    images.qrc

OTHER_FILES += \
    CMakeLists.txt
