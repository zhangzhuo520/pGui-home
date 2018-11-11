INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gui/renderworker/renderer)

SET(QT_USE_SQL TRUE)

SET(PicathoGui_RESOURCES images.qrc)

SET(PicathoGui_SRCS
    main.cc 
    mainwindow.cc 
    render_frame.cc
    checklist.cc 
    gearmanconnector.cc
    drawwidget.cc
    defgroup.cc
    defectswidget.cc		   
    frame.cc
    menuwidget.cc
    fileprojectwidget.cc
    layerwidget.cc
    deftools/cmessagebox.cc 
    deftools/defcontrols.cc
    deftools/framelesshelper.cc
    deftools/global.cc
    deftools/model.cc
    db/sqlquery.cc
    db/sqlmanager.cc)

SET(PicathoGui_HEADERS
    mainwindow.h
    render_frame.h 
    checklist.h 
    gearmanconnector.h
    drawwidget.h 
    defgroup.h
    frame.h
    layerwidget.h
    defectswidget.h
    menuwidget.h
    fileprojectwidget.h
    deftools/cmessagebox.h 
    deftools/framelesshelper.h
    deftools/defcontrols.h
    deftools/datastruct.h 
    deftools/global.h
    deftools/model.h
    db/sqlquery.h
    db/sqlmanager.h)

QT4_WRAP_CPP(PicathoGui_MOCS
    mainwindow.h
    checklist.h
    drawwidget.h
    fileprojectwidget.h
    layerwidget.h
    defgroup.h
    frame.h
    menuwidget.h
    defectswidget.h
    defgroup.h
    deftools/cmessagebox.h 
    deftools/framelesshelper.h
    deftools/defcontrols.h
    db/sqlmanager.h)

QT4_ADD_RESOURCES(PicathoGui_RESOURCES_RCC ${PicathoGui_RESOURCES})

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(PicathoGui
    ${PicathoGui_HEADERS}
    ${PicathoGui_RESOURCES}
    ${PicathoGui_RESOURCES_RCC}
    ${PicathoGui_SRCS}
    ${PicathoGui_MOCS})

TARGET_LINK_LIBRARIES(PicathoGui renderer ${QT_LIBRARIES})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
