INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gui/renderworker/renderer)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/component/infra/gdshandling/oasis)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gui/qt_logger)


SET(QT_USE_SQL TRUE)

SET(PicathoGui_RESOURCES images.qrc)

SET(PicathoGui_SRCS
    main.cc
    ui_mainwindow.cc
    ui_checklist.cc
    ui_defgroup.cc
    ui_paint_widget.cc
    ui_tab_paintwidget.cc
    ui_paint_toolbar.cc
    ui_defects_widget.cc
    ui_layer_widget.cc
    ui_fileproject_widget.cc
    ui_gaugetable.cc
    ui_frame.cc
    ui_measurepoint.cc
    ui_measure_table.cc
    ui_menu_widget.cc
    ui_choosefile_dialog.cc
    ui_semimage_dialog.cc
    ui_chipedit_dialog.cc
    ui_scale_frame.cc
    ui_application.cc
    ui_log_widget.cc
    rts_setup/ui_rtsmask_tab.cc
    rts_setup/ui_rtscurve.cc
    rts_setup/ui_rtsreview_dialog.cc
    rts_setup/ui_rtsconfig_dialog.cc
    rts_setup/ui_rtsreview_table.cc
    rts_setup/rts_runprocess.cc
    deftools/cmessagebox.cc
    deftools/defcontrols.cc
    deftools/framelesshelper.cc
    deftools/global.cc
    deftools/iconhelper.cc
    deftools/delegate.cc
    deftools/showcpumemory.cc
    delegate/ui_rtsmask_delegate.cc
    db/sqlquery.cc
    db/sqlmanager.cc
    fileparsing/yamlparsing.cc
    fileparsing/rts_imageparsing.cc
    file_generate/rts_pythonwriter.cc
    model/ui_checklist_model.cc
    model/ui_measuretable_model.cc
    model/ui_sqltabel_model.cc
    model/ui_gauge_model.cc  
    model/ui_rtsmask_model.cc
    model/ui_universal_model.cc
    model/ui_fileproject_model.cc)


SET(PicathoGui_HEADERS
    ui_mainwindow.h
    ui_checklist.h
    ui_defgroup.h
    ui_paint_toolbar.h
    ui_gaugetable.h
    ui_paint_widget.h
    ui_tab_paintwidget.h
    ui_defects_widget.h
    ui_fileproject_widget.h
    ui_layer_widget.h
    ui_log_widget.h
    ui_frame.h
    ui_measurepoint.h
    ui_measure_table.h
    ui_menu_widget.h
    ui_choosefile_dialog.h
    ui_semimage_dialog.h
    ui_chipedit_dialog.h
    ui_application.h
    rts_setup/ui_rtsmask_tab.h
    rts_setup/ui_rtscurve.h
    rts_setup/ui_rtsconfig_dialog.h
    rts_setup/ui_rtsreview_dialog.h
    rts_setup/rts_runprocess.h
    rts_setup/ui_rtsreview_table.h
    deftools/cmessagebox.h
    deftools/framelesshelper.h
    deftools/iconhelper.h
    deftools/defcontrols.h
    deftools/datastruct.h
    deftools/delegate.h
    deftools/global.h
    deftools/showcpumemory.h
    delegate/ui_rtsmask_delegate.h
    db/sqlquery.h
    db/sqlmanager.h
    fileparsing/yamlparsing.h
    fileparsing/rts_imageparsing.h
    file_generate/rts_pythonwriter.h
    model/ui_checklist_model.h
    model/ui_measuretable_model.h
    model/ui_gauge_model.h  
    model/ui_sqltabel_model.h
    model/ui_rtsmask_model.h
    model/ui_universal_model.h
    model/ui_fileproject_model.h)

QT4_WRAP_CPP(PicathoGui_MOCS
    ui_mainwindow.h
    ui_scale_frame.h
    ui_gaugetable.h
    ui_tab_paintwidget.h
    ui_measure_table.h
    ui_log_widget.h
    ui_semimage_dialog.h
    ui_chipedit_dialog.h
    ui_choosefile_dialog.h
    ui_checklist.h
    ui_paint_widget.h
    ui_paint_toolbar.h
    ui_fileproject_widget.h
    ui_layer_widget.h
    ui_defgroup.h
    ui_frame.h
    ui_menu_widget.h
    ui_defects_widget.h
    ui_defgroup.h
    rts_setup/ui_rtscurve.h
    rts_setup/ui_rtsconfig_dialog.h
    rts_setup/ui_rtsmask_tab.h
    rts_setup/ui_rtsreview_dialog.h
    rts_setup/rts_runprocess.h
    rts_setup/ui_rtsreview_table.h
    fileparsing/rts_imageparsing.h
    deftools/cmessagebox.h
    deftools/framelesshelper.h
    deftools/defcontrols.h
    deftools/iconhelper.h
    deftools/showcpumemory.h
    deftools/delegate.h
    delegate/ui_rtsmask_delegate.h
    db/sqlmanager.h
    model/ui_measuretable_model.h
    model/ui_checklist_model.h
    model/ui_gauge_model.h  
    model/ui_fileproject_model.h
    model/ui_rtsmask_model.h
    model/ui_universal_model.h
    model/ui_sqltabel_model.h)

QT4_ADD_RESOURCES(PicathoGui_RESOURCES_RCC ${PicathoGui_RESOURCES})

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(PicathoGui
    ${PicathoGui_HEADERS}
    ${PicathoGui_RESOURCES}
    ${PicathoGui_RESOURCES_RCC}
    ${PicathoGui_SRCS}
    ${PicathoGui_MOCS})

TARGET_LINK_LIBRARIES(PicathoGui renderer oasis_api sysutil qt_logger ${QT_LIBRARIES})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
