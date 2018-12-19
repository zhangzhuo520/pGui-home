INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gui/renderworker/renderer)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/component/infra/gdshandling/oasis)

SET(QT_USE_SQL TRUE)

SET(PicathoGui_RESOURCES images.qrc)

SET(PicathoGui_SRCS
    main.cc
    ui_mainwindow.cc
    gearmanconnector.cc
    ui_checklist.cc
    ui_defgroup.cc
    ui_paint_widget.cc
    ui_paint_toolbar.cc
    ui_defects_widget.cc
    ui_layer_widget.cc
    ui_fileproject_widget.cc
    ui_fileproject_model.cc
    ui_frame.cc
    ui_measurepoint.cc
    ui_menu_widget.cc
    ui_rtsconfig_dialog.cc
    ui_choosefile_dialog.cc
    ui_semimage_dialog.cc
    ui_chipedit_dialog.cc
    ui_scale_frame.cc
    ui_application.cc
    deftools/cmessagebox.cc
    deftools/defcontrols.cc
    deftools/framelesshelper.cc
    deftools/global.cc
    deftools/iconhelper.cc
    deftools/model.cc
    deftools/delegate.cc
    db/sqlquery.cc
    db/sqlmanager.cc)

SET(PicathoGui_HEADERS
    ui_mainwindow.h
    gearmanconnector.h
    ui_checklist.h
    ui_defgroup.h
    ui_paint_toolbar.h
    ui_paint_widget.h
    ui_defects_widget.h
    ui_fileproject_widget.h
    ui_fileproject_model.h
    ui_layer_widget.h
    ui_frame.h
    ui_measurepoint.h
    ui_menu_widget.h
    ui_rtsconfig_dialog.h
    ui_choosefile_dialog.h
    ui_semimage_dialog.h
    ui_chipedit_dialog.h
    ui_application.h
    deftools/cmessagebox.h
    deftools/framelesshelper.h
    deftools/iconhelper.h
    deftools/defcontrols.h
    deftools/datastruct.h
    deftools/delegate.h
    deftools/global.h
    deftools/model.h
    db/sqlquery.h
    db/sqlmanager.h)

QT4_WRAP_CPP(PicathoGui_MOCS
    ui_mainwindow.h
    ui_rtsconfig_dialog.h
    ui_scale_frame.h
    ui_semimage_dialog.h
    ui_chipedit_dialog.h
    ui_choosefile_dialog.h
    ui_checklist.h
    ui_paint_widget.h
    ui_paint_toolbar.h
    ui_fileproject_widget.h
    ui_fileproject_model.h
    ui_layer_widget.h
    ui_defgroup.h
    ui_frame.h
    ui_menu_widget.h
    ui_defects_widget.h
    ui_defgroup.h
    deftools/cmessagebox.h
    deftools/framelesshelper.h
    deftools/defcontrols.h
    deftools/iconhelper.h
    deftools/delegate.h
    db/sqlmanager.h)

QT4_ADD_RESOURCES(PicathoGui_RESOURCES_RCC ${PicathoGui_RESOURCES})

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(PicathoGui
    ${PicathoGui_HEADERS}
    ${PicathoGui_RESOURCES}
    ${PicathoGui_RESOURCES_RCC}
    ${PicathoGui_SRCS}
    ${PicathoGui_MOCS})

TARGET_LINK_LIBRARIES(PicathoGui renderer oasis_api sysutil ${QT_LIBRARIES})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
