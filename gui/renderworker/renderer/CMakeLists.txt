SET(SOURCE_FILES 
    render_snap.cc
    render_frame.cc
    render_layout_view.cc
    render_defect_point.cc
    render_view_object.cc
    render_worker.cc
    render_bitmaps_to_image.cc
    render_layer_properties.cc
    render_layer_metadata.cc
    render_view_op.cc
    render_pattern.cc
    render_line_style.cc
    render_viewport.cc
    render_palette.cc
)

QT4_WRAP_CPP(RENDERER_MOCS render_frame.h render_pattern.h render_line_style.h)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY(renderer ${RENDERER_MOCS} ${SOURCE_FILES})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
TARGET_LINK_LIBRARIES(renderer oasis qt_logger ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
