#ifndef OASIS_PARSER_H
#define OASIS_PARSER_H
#include "parser_base.h"
#include "oasis_reader.h"
#include "oasis_record.h"
#include "oasis_modal_variable.h"
#include <string>

namespace oasis
{

class OasisLayout;
class OasisCell;
class OasisCellInfo;
class OasisElement;
class OasisParser : public ParserBase
{
public:
    OasisParser();
    ~OasisParser();

    void open_file(std::string filename);
    void parse_file(OasisLayout* layout);
    void set_file_offset(uint64 offset, uint64 hint_size);
    void parse_magic();
    void parse_start();
    void parse_end();
    void parse_all();
    void parse_cell_second_pass(int64 cell_index);
    void parse_cell(const OasisRecordId& rid, int64 cell_index);
    void parse_cell_name(const OasisRecordId& rid, bool skip = false);
    void parse_text_string(const OasisRecordId& rid, bool skip = false);
    void parse_layer_name(const OasisRecordId& rid, bool skip = false);
    void parse_prop_name(const OasisRecordId& rid, bool skip = false);
    void parse_prop_string(const OasisRecordId& rid, bool skip = false);
    void parse_x_name(const OasisRecordId& rid, bool skip = false);

    virtual void parse_cell_concurrent(int64 info_index);

    void load_cell(OasisLayout* layout, int64 infoindex);
    void import_file(OasisLayout* layout);
    void import_tables();
    void parse_cell_name_table();
    void parse_text_string_table();
    void parse_prop_name_table();
    void parse_prop_string_table();
    void parse_layer_name_table();
    void parse_x_name_table();
    
    bool check_file(OasisLayout* layout, uint32* sourcefile_sha1 = NULL);
    bool check_format();

private:
    OasisRecordId read_record_id();
    void read_placement(const OasisRecordId& rid, OasisCell& cell);
    void read_text(const OasisRecordId& rid, OasisCell& cell);
    void read_rectangle(const OasisRecordId& rid, OasisCell& cell);
    void read_polygon(const OasisRecordId& rid, OasisCell& cell);
    void read_path(const OasisRecordId& rid, OasisCell& cell);
    void read_trapezoid(const OasisRecordId& rid, OasisCell& cell);
    void read_c_trapezoid(const OasisRecordId& rid, OasisCell& cell);
    void read_circle(const OasisRecordId& rid, OasisCell& cell);
    void read_properties(const OasisRecordId& rid);
    void read_x_element(const OasisRecordId& rid, OasisCell& cell);
    void read_x_geometry(const OasisRecordId& rid, OasisCell& cell);
    void enter_cblock(const OasisRecordId& rid);
    void read_element_properties();
    void read_cell_properties();
    void read_cell_name_properties(OasisCellInfo& cellinfo);
    void skip_name_properties();
    void read_file_properties();
    void read_table_offsets();

    void cache_id(bool enable = true) { m_use_cached_id = enable; }

    static bool is_generated_cellname(const std::string& name);
    static std::string rename_generated_prefix(const std::string& name);
private:
    FILE* m_openfp;
    OasisReader m_reader;
    ModalVariable m_modal_variables;
    OasisRecordId m_cached_id;
    bool m_use_cached_id;
    bool m_offsets_at_end;
    bool m_cellprop_uselayers;

    enum IdMode
    {
        IMPLICIT,
        EXPLICIT,
        UNKNOWN
    };
    IdMode m_cellname_idmode;
    IdMode m_textstring_idmode;
    IdMode m_propstring_idmode;
    IdMode m_propname_idmode;
    IdMode m_xname_idmode;

    uint64 m_cellname_refnum;
    uint64 m_textstring_refnum;
    uint64 m_propname_refnum;
    uint64 m_propstring_refnum;
    uint64 m_xname_refnum;

    enum ParseMode
    {
        PARSE,
        IMPORT,
        RELOAD,
        SECOND_PASS
    };

    ParseMode m_parse_mode;
};

}

#endif
