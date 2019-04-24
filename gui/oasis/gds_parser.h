#ifndef GDS_PARSER_H
#define GDS_PARSER_H
#include "parser_base.h"
#include "oasis_types.h"
#include "gds_reader.h"

namespace oasis
{
class GDSRecord
{
public:
    GDSRecord() : m_record_len(0), m_record_id(GRID_Max), m_current(0), m_end(0) {}
    void read(GDSReader& reader);

    uint16 get_record_length() const { return m_record_len; }
    GDSRecordId get_record_id() const { return m_record_id; }

    std::string get_string();
    uint16 get_uint16();
    uint32 get_uint32();
    int16 get_int16();
    int32 get_int32();
    float64 get_float64();
    void get_xy(std::vector<Point>& points);

    int64 get_offset() const { return m_offset; }

private:

private:
    int64 m_offset;
    uint16 m_record_len;
    GDSRecordId m_record_id;

    uint8 m_buffer[65536];
    uint8* m_current;
    uint8* m_end;
};

class OasisLayout;
class OasisCell;
class GDSParser : public ParserBase
{
public:
    enum ParseMode { FIRST_PASS, SECOND_PASS};
    GDSParser();
    ~GDSParser();

    void open_file(const std::string& filename);
    void set_file_offset(int64 offset, int64 hint_size);
    void parse_file(OasisLayout* layout);
    void parse_start();
    void parse_all();
    void parse_end();
    void parse_cell(int64 cell_index);
    void parse_cell_second_pass(int64 cell_index);

    virtual void parse_cell_concurrent(int64 info_index);

    bool check_format();

private:
    GDSRecordId read_record();
    void read_boundary(OasisCell& cell);
    void read_path(OasisCell& cell);
    void read_ref(OasisCell& cell, bool array);
    void read_text(OasisCell& cell);
    void read_node(OasisCell& cell);
    void read_box(OasisCell& cell);
    void read_property();
    void cached_record(bool enable = true) { m_use_cached_record = enable; }

    void add_array(OasisCell& cell, int64 infoindex, const OasisTrans &trans, int64 cols, int64 rows, const Point &cv, const Point &rv);

private:
    ParseMode m_parse_mode;
    FILE* m_fp;
    GDSReader m_reader;

    bool m_use_cached_record;
    GDSRecord m_record;
};

}

#endif
