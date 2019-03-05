SET(OASIS_SOURCES
  oasis_types.cc
  oasis_record.cc
  oasis_reader.cc
  oasis_writer.cc
  oasis_parser.cc
  oasis_out_stream.cc
  oasis_modal_variable.cc
  oasis_decompressor.cc
  oasis_compressor.cc
  oasis_layout.cc
  oasis_cell.cc
  oasis_util.cc
  gds_reader.cc
  gds_parser.cc
  oasis_parse_option.cc
  cell_visitor.cc
  fetch_polygons_visitor.cc
  fetch_instance_visitor.cc
  parser_base.cc
  render_bitmap.cc
  draw_bitmap_visitor.cc
  snap_edge_visitor.cc
  count_mark_visitor.cc
  select_polygons_visitor.cc
)

ADD_LIBRARY(oasis ${OASIS_SOURCES})
TARGET_LINK_LIBRARIES(oasis
  db
  z
)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

ADD_EXECUTABLE(oas oasis.cc)
TARGET_LINK_LIBRARIES(oas oasis sysutil ${BOOST_LIBRARY_DIR}/libboost_system.a ${BOOST_LIBRARY_DIR}/libboost_filesystem.a)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test)

