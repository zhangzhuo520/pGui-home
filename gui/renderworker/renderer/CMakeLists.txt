SET(SOURCE_FILES 
    render_bitmaps_to_image.cc
    render_view_op.cc
    render_pattern.cc
    render_line_style.cc
)

QT4_WRAP_CPP(RENDERER_MOCS render_pattern.h render_line_style.h)

ADD_LIBRARY(renderer ${RENDERER_MOCS} ${SOURCE_FILES})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
TARGET_LINK_LIBRARIES(renderer oasis ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
