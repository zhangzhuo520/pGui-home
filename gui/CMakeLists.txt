SET(USE_QT_SQL TRUE)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/component/infra/gdshandling/oasis)

ADD_SUBDIRECTORY(qt_logger)
ADD_SUBDIRECTORY(renderworker)
ADD_SUBDIRECTORY(ui_widget)

