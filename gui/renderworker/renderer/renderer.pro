HEADERS += \
    render_worker.h \
    render_viewport.h \
    render_view_op.h \
    render_pattern.h \
    render_bitmaps_to_image.h \
    render_layer_metadata.h \
    render_layer_properties.h \
    render_line_style.h \
    render_palette.h \
    render_layout_view.h \
    render_frame.h \
    render_defect_point.h \
    render_view_object.h \
    render_snap.h \
    render_monitor_thread.h \
    render_image.h \
    render_global.h \
    render_bitmap_manager.h

SOURCES += \
    render_worker.cc \
    render_viewport.cc \
    render_view_op.cc \
    render_bitmaps_to_image.cc \
    render_layer_metadata.cc \
    render_layer_properties.cc \
    render_line_style.cc \
    render_palette.cc \
    render_pattern.cc \
    render_layout_view.cc \
    render_frame.cc \
    render_defect_point.cc \
    render_view_object.cc \
    render_snap.cc \
    render_monitor_thread.cc \
    render_image.cc \
    render_bitmap_manager.cc

OTHER_FILES += \
    CMakeLists.txt
