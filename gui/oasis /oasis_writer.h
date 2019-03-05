#ifndef OASIS_WRITER_H
#define OASIS_WRITER_H
#include "oasis_types.h"
#include "oasis_record.h"
#include "oasis_modal_variable.h"
#include "string.h"

namespace oasis
{

class OasisCompressor;
class WriteBuffer
{
public:
    WriteBuffer() : m_size(0) {}
    WriteBuffer(uint32 size) : m_size(size) { m_buffer.reserve(size); }

    uint32 size() const { return m_size; }
    void resize(uint32 size) { m_size = size; m_buffer.reserve(size); }
    void flush() { m_buffer.clear(); }
    bool full() const { return m_buffer.size() >= m_size; }
    uint32 used() const { return m_buffer.size(); }
    uint32 free() const { return m_size > m_buffer.size() ? m_size - m_buffer.size() : 0; }
    void write(uint8 byte) { m_buffer.push_back(byte); }
    void write(const uint8* data, uint32 size) { m_buffer.insert(m_buffer.end(), data, data + size); }
    const uint8* data() const { return m_size ? &m_buffer[0] : NULL; }
private:
    std::vector<uint8> m_buffer;
    uint32 m_size;
};

class OasisWriterOption
{
public:
    OasisWriterOption() : 
        m_enable_file_property(true),
        m_enbale_cell_property(true),
        m_enable_element_property(false) {}
    bool enable_file_property() const { return m_enable_file_property; }
    bool enable_cell_property() const { return m_enbale_cell_property; }
    bool enable_element_property() const { return m_enable_element_property; }
private:
    bool m_enable_file_property;
    bool m_enbale_cell_property;
    bool m_enable_element_property;
};

class OasisWriter
{
public:
    OasisWriter();
    virtual ~OasisWriter();

    void set_option(const OasisWriterOption& option) { m_option = option; }
    const OasisWriterOption& get_option() const { return m_option; }

    void write_byte(uint8 byte);
    void write_bytes(const uint8 *buf, int32 len);

    void write_uint(uint64 value);
    void write_int(int64 value);

    void write_string(const std::string& str);
    void write_string(const char* data, uint32 len);
    void write_string(const char* str);
    void write_string(const OasisString& ostring);
    void write_real(const OasisReal& real);
    void write_delta_1(const OasisDelta& delta, bool horizontal);
    void write_delta_2(const OasisDelta& delta);
    void write_delta_3(const OasisDelta& delta);
    void write_delta_g(const OasisDelta& delta);
    void write_repetition(const OasisRepetition& repetition);
    void write_point_list(const OasisPointList& pointlist);
    void write_prop_value(const OasisPropValue& propvalue);
    void write_interval(const OasisInterval& interval);

    void write_record_id(const OasisRecordId& rid);
    void write_magic();

    ModalVariable& get_modal_variables() { return m_modal_variables; }
    void enter_cblock();
    void leave_cblock();
    void enable_cblock(bool enable = true) { m_enable_cblock = enable; }
    bool enable_cblock() const { return m_enable_cblock; }

    void set_file_offset(FILE* fp, uint64 offset) { m_fp = fp; m_fileOffset = offset; }
    uint64 get_offset() { oas_assert(!m_in_cblock); return m_fileOffset + m_file_buffer.used(); }
    virtual void flush_buffer(bool atend = false);

    sys::Mutex& get_locker() { return m_locker; }

protected:
    bool buffer_full() { return m_file_buffer.full(); }
    int32 buffer_free() { return m_file_buffer.free(); }

protected:
    FILE* m_fp;
    WriteBuffer m_file_buffer;
    WriteBuffer m_cblock_buffer;
    WriteBuffer m_compressed_buffer;
    uint64 m_fileOffset;

    OasisCompressor* m_compressor;
    bool m_in_cblock;
    bool m_enable_cblock;


    ModalVariable m_modal_variables;

    OasisWriterOption m_option;

    sys::Mutex m_locker;
};

class MemoryOasisWriter : public OasisWriter
{
public:
    MemoryOasisWriter();
    MemoryOasisWriter(const OasisWriter& base_writer);
    virtual void flush_buffer(bool atend = false);
    const uint8* get_data() const;
    const uint32 get_size() const;
};

}

#endif
