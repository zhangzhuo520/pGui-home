#include "oasis_writer.h"
#include "oasis_compressor.h"
#include "oasis_exception.h"

using namespace std;
using namespace oasis;

#define FILE_BUF_SIZE (1024 * 1024)
OasisWriter::OasisWriter() :
    m_fp(NULL),
    m_file_buffer(FILE_BUF_SIZE),
    m_cblock_buffer(FILE_BUF_SIZE),
    m_compressed_buffer(FILE_BUF_SIZE),
    m_fileOffset(0),
    m_in_cblock(false),
    m_enable_cblock(true)
{
    m_compressor = new OasisCompressor;
    if(m_compressor->initialize() < 0)
    {
        throw OasisException("compressor initialize failed");
    }
}

OasisWriter::~OasisWriter()
{
    delete m_compressor;
}

void OasisWriter::enter_cblock()
{
    if(!m_enable_cblock)
    {
        return;
    }
    OAS_DBG << "enter CBlock" << OAS_ENDL;
    oas_assert(!m_in_cblock);
    m_in_cblock = true;
}

void OasisWriter::leave_cblock()
{
    if(!m_enable_cblock)
    {
        return;
    }
    OAS_DBG << "leave CBlock" << OAS_ENDL;
    oas_assert(m_in_cblock);

    m_in_cblock = false;

    if(m_cblock_buffer.used() == 0)
    {
        OAS_DBG << "no compress data" << OAS_ENDL;
        return;
    }

    uint8 *bufout = new uint8[m_cblock_buffer.used()];
    uint32 bufsize = m_cblock_buffer.used();

    if(m_compressor->prepare_compress() < 0)
    {
        delete [] bufout;
        throw OasisException("begin Cblock failed");
    }
    m_compressor->feed_all_data(m_cblock_buffer.data(), m_cblock_buffer.used());

    uint32 fetchedSize;
    m_compressed_buffer.flush();
    do
    {
        if (m_compressor->fetch_data(bufout, bufsize, fetchedSize) < 0)
        {
            delete [] bufout;
            throw OasisException("get compressed data failed");
        }
        m_compressed_buffer.write(bufout, fetchedSize);
    } while (fetchedSize > 0);

    delete [] bufout;

    write_record_id(ORID_Cblock);
    write_uint(0);
    write_uint(m_cblock_buffer.used());
    write_uint(m_compressed_buffer.used());
    write_bytes(m_compressed_buffer.data(), m_compressed_buffer.used());

    m_cblock_buffer.flush();
    m_compressed_buffer.flush();
}

void OasisWriter::flush_buffer(bool atend)
{
    if(m_file_buffer.used() == 0)
    {
        return;
    }
    uint32 n = fwrite(m_file_buffer.data(), 1, m_file_buffer.used(), m_fp);
    if (n != m_file_buffer.used())
    {
        OAS_ERR << "flush data failed. buflen " << m_file_buffer.used() << ", write n " << n << OAS_ENDL;
        perror("flush data failed.");
        throw OasisException("write file failed");
    }
    m_fileOffset += m_file_buffer.used();

    m_file_buffer.flush();

    //we intentionally write a dummy record id
    //when we reload from the file, we need to know the next rid
    if(!atend)
    {
        uint8 dummyrid = ORID_XName;
        n = fwrite(&dummyrid, 1, 1, m_fp);
        if(n != 1)
        {
            OAS_ERR << "flush dummy record failed. n " << n << OAS_ENDL;
            perror("flush dummy record failed.");
            throw OasisException("write file failed");
        }
    }
    fflush(m_fp);

    if(!atend)
    {
        fseek(m_fp, -1, SEEK_CUR);
    }
}

void OasisWriter::write_byte(uint8 byte)
{
    if(m_in_cblock)
    {
        m_cblock_buffer.write(byte);
        return;
    }

    if(buffer_full())
    {
        flush_buffer();
    }
    m_file_buffer.write(byte);
}

void OasisWriter::write_bytes(const uint8* buf, int32 len)
{
    if(m_in_cblock)
    {
        m_cblock_buffer.write(buf, len);
        return;
    }

    while(len > 0)
    {
        if(buffer_full())
        {
            flush_buffer();
        }

        int32 n = std::min(len, buffer_free());
        m_file_buffer.write(buf, n);
        buf += n;
        len -= n;
    }
}

void OasisWriter::write_uint(uint64 value)
{
    while (value > 0x7F)
    {
        write_byte(value | 0x80);
        value >>= 7;
    }
    write_byte(value);
}

void OasisWriter::write_int(int64 value)
{
    write_uint(int_to_uint(value));
}

void OasisWriter::write_string(const char* data, uint32 len)
{
    write_uint(len);
    write_bytes(reinterpret_cast<const uint8*>(data), len);
}

void OasisWriter::write_string(const char* str)
{
    int32 len = strlen(str);
    write_string(str, len);
}

void OasisWriter::write_string(const string& str)
{
    write_string(str.c_str(), str.size());
}

void OasisWriter::write_string(const OasisString& ostring)
{
    write_string(ostring.value());
}

void OasisWriter::write_real(const OasisReal& real)
{
    write_uint(real.m_type);
    switch(real.m_type)
    {
        case OasisReal::ORT_PositiveInteger:
            write_uint(real.m_numerator);
            break;
        case OasisReal::ORT_NegativeInteger:
            write_uint(real.m_numerator);
            break;
        case OasisReal::ORT_PositiveReciprocal:
            write_uint(real.m_denominator);
            break;
        case OasisReal::ORT_NegativeReciprocal:
            write_uint(real.m_denominator);
            break;
        case OasisReal::ORT_PositiveRatio:
            write_uint(real.m_numerator);
            write_uint(real.m_denominator);
            break;
        case OasisReal::ORT_NegativeRatio:
            write_uint(real.m_numerator);
            write_uint(real.m_denominator);
            break;
        case OasisReal::ORT_Float32:
            {
            float32 value = real.m_value;
            write_bytes((uint8*)&value, sizeof(value));
            }
            break;
        case OasisReal::ORT_Float64:
            {
            float64 value = real.m_value;
            write_bytes((uint8*)&value, sizeof(value));
            }
            break;
        default:
            //error
            break;
    }
    return;
}

void OasisWriter::write_delta_1(const OasisDelta& delta, bool horizontal)
{
    oas_assert(delta.m_type == OasisDelta::ODT_Delta1);
    if(horizontal)
    {
        write_int(delta.m_dx);
    }
    else
    {
        write_int(delta.m_dy);
    }
}

void OasisWriter::write_delta_2(const OasisDelta& delta)
{
    oas_assert(delta.m_type == OasisDelta::ODT_Delta2);
    uint64 value;
    OasisDelta::Direction dir;
    delta.getValue(dir, value);
    value = ((value << 2) | (dir & 0x3));
    write_uint(value);
}

void OasisWriter::write_delta_3(const OasisDelta& delta)
{
    oas_assert(delta.m_type == OasisDelta::ODT_Delta3);
    uint64 value;
    OasisDelta::Direction dir;
    delta.getValue(dir, value);
    value = (value << 3) | (dir & 0x07);
    write_uint(value);
    return;
}

void OasisWriter::write_delta_g(const OasisDelta& delta)
{
    oas_assert(delta.m_type == OasisDelta::ODT_DeltaG);
    uint64 value;
    OasisDelta::Direction dir;
    delta.getValue(dir, value);
    if(dir != OasisDelta::Any)
    {
        value = (value << 4) | (dir << 1);
        write_uint(value);
        return;
    }

    value = ((int_to_uint(delta.m_dx) << 1) | 1);
    write_uint(value);
    write_int(delta.m_dy);
    return;
}

void OasisWriter::write_interval(const OasisInterval& interval)
{
    write_uint(interval.m_type);
    switch(interval.m_type)
    {
        case OasisInterval::OIT_All:
            break;
        case OasisInterval::OIT_BoundBellow:
            write_uint(interval.m_upper);
            break;
        case OasisInterval::OIT_BoundAbove:
            write_uint(interval.m_lower);
            break;
        case OasisInterval::OIT_Exact:
            write_uint(interval.m_lower);
            break;
        case OasisInterval::OIT_BoundAll:
            write_uint(interval.m_lower);
            write_uint(interval.m_upper);
            break;
        default:
            //error
            break;
    }
}

void OasisWriter::write_point_list(const OasisPointList& pointlist)
{
    write_uint(pointlist.m_type);

    write_uint(pointlist.m_deltas.size());
    bool horizontal = false;
    switch(pointlist.m_type)
    {
        case OasisPointList::OPLT_ManhattanHorizontalFirst:
            horizontal = true;
        case OasisPointList::OPLT_ManhattanVerticalFirst:
            for(uint64 i = 0; i < pointlist.m_deltas.size(); ++i)
            {
                write_delta_1(pointlist.m_deltas[i], horizontal);
                horizontal = !horizontal;
            }
            break;
        case OasisPointList::OPLT_Manhattan:
            for(uint64 i = 0; i < pointlist.m_deltas.size(); ++i)
            {
                write_delta_2(pointlist.m_deltas[i]);
            }
            break;
        case OasisPointList::OPLT_Octangular:
            for(uint64 i = 0; i < pointlist.m_deltas.size(); ++i)
            {
                write_delta_3(pointlist.m_deltas[i]);
            }
            break;
        case OasisPointList::OPLT_AnyAngle:
        case OasisPointList::OPLT_AnyAngleAdjust:
            for(uint64 i = 0; i < pointlist.m_deltas.size(); ++i)
            {
                write_delta_g(pointlist.m_deltas[i]);
            }
            break;
        default:
            //error
            break;
    }
    return;
}

void OasisWriter::write_repetition(const OasisRepetition& repetition)
{
    write_uint(repetition.m_type);
    switch(repetition.m_type)
    {
        case OasisRepetition::ORT_ReusePrevious:
            oas_assert(false);
            break;
        case OasisRepetition::ORT_Regular2D:
            write_uint(repetition.m_dimensionX);
            write_uint(repetition.m_dimensionY);
            oas_assert(repetition.m_spaces.size() == 2);
            write_uint(repetition.m_spaces[0]);
            write_uint(repetition.m_spaces[1]);
            break;
        case OasisRepetition::ORT_HorizontalRegular:
            write_uint(repetition.m_dimensionX);
            if (repetition.m_spaces.size() > 1) {
                OAS_WARN << "OasisRepetition::ORT_HorizontalRegular: repetition.m_spaces.size>1. Size=" << repetition.m_spaces.size() << OAS_ENDL;
            }
            write_uint(repetition.m_spaces[0]);
            break;
        case OasisRepetition::ORT_VerticalRegular:
            write_uint(repetition.m_dimensionY);
            if (repetition.m_spaces.size() > 1) {
                OAS_WARN << "OasisRepetition::ORT_VerticalRegular: repetition.m_spaces.size>1. Size=" << repetition.m_spaces.size() << OAS_ENDL;
            }
            write_uint(repetition.m_spaces[0]);
            break;
        case OasisRepetition::ORT_HorizontalIrregular:
            write_uint(repetition.m_dimensionX);
            oas_assert(repetition.m_spaces.size() == repetition.m_dimensionX + 1);
            for(uint32 i = 0; i < repetition.m_spaces.size(); ++i)
            {
                write_uint(repetition.m_spaces[i]);
            }
            break;
        case OasisRepetition::ORT_HorizontalIrregularGrid:
            write_uint(repetition.m_dimensionX);
            write_uint(repetition.m_grid);
            oas_assert(repetition.m_spaces.size() == repetition.m_dimensionX + 1);
            for(uint32 i = 0; i < repetition.m_spaces.size(); ++i)
            {
                write_uint(repetition.m_spaces[i]);
            }
            break;
        case OasisRepetition::ORT_VerticalIrregular:
            write_uint(repetition.m_dimensionY);
            oas_assert(repetition.m_spaces.size() == repetition.m_dimensionY + 1);
            for(uint32 i = 0; i < repetition.m_spaces.size(); ++i)
            {
                write_uint(repetition.m_spaces[i]);
            }
            break;
        case OasisRepetition::ORT_VerticalIrregularGrid:
            write_uint(repetition.m_dimensionY);
            write_uint(repetition.m_grid);
            oas_assert(repetition.m_spaces.size() == repetition.m_dimensionY + 1);
            for(uint32 i = 0; i < repetition.m_spaces.size(); ++i)
            {
                write_uint(repetition.m_spaces[i]);
            }
            break;
        case OasisRepetition::ORT_DiagonalRegular2D:
            write_uint(repetition.m_dimensionN);
            write_uint(repetition.m_dimensionM);
            oas_assert(repetition.m_deltas.size() == 2);
            write_delta_g(repetition.m_deltas[0]);
            write_delta_g(repetition.m_deltas[1]);
            break;
        case OasisRepetition::ORT_DiagonalRegular1D:
            write_uint(repetition.m_dimension);
            oas_assert(repetition.m_deltas.size() == 1);
            write_delta_g(repetition.m_deltas[0]);
            break;
        case OasisRepetition::ORT_Arbitory:
            write_uint(repetition.m_dimension);
            oas_assert(repetition.m_deltas.size() == repetition.m_dimension + 1);
            for(uint32 i = 0; i < repetition.m_deltas.size(); ++i)
            {
                write_delta_g(repetition.m_deltas[i]);
            }
            break;
        case OasisRepetition::ORT_ArbitoryGrid:
            write_uint(repetition.m_dimension);
            write_uint(repetition.m_grid);
            oas_assert(repetition.m_deltas.size() == repetition.m_dimension + 1);
            for(uint32 i = 0; i < repetition.m_deltas.size(); ++i)
            {
                write_delta_g(repetition.m_deltas[i]);
            }
            break;
        default:
            //error
            break;
    }
    return;
}

void OasisWriter::write_prop_value(const OasisPropValue& propvalue)
{
    if(propvalue.m_type < static_cast<OasisPropValue::Type>(OasisReal::ORT_Max))
    {
        write_real(propvalue.m_real_value);
        return;
    }

    write_uint(propvalue.m_type);

    switch(propvalue.m_type)
    {
        case OasisPropValue::OPVT_UnsignedInteger:
            write_uint(propvalue.m_uint_value);
            break;
        case OasisPropValue::OPVT_SignedInteger:
            write_int(propvalue.m_int_value);
            break;
        case OasisPropValue::OPVT_AString:
            write_string(propvalue.m_string_value);
            break;
        case OasisPropValue::OPVT_BString:
            write_string(propvalue.m_string_value);
            break;
        case OasisPropValue::OPVT_NString:
            write_string(propvalue.m_string_value);
            break;
        case OasisPropValue::OPVT_RefAString:
        case OasisPropValue::OPVT_RefBString:
        case OasisPropValue::OPVT_RefNString:
            write_uint(propvalue.m_ref_value);
            break;
        default:
            throw OasisException("write invalid propvalue type");
            break;
    }
    return;
}

void OasisWriter::write_record_id(const OasisRecordId& rid)
{
    if(m_in_cblock && m_cblock_buffer.full())
    {
        leave_cblock();
        enter_cblock();
    }
    write_byte(static_cast<uint8>(rid));
    OAS_DBG << "write record " << oasis_record_id_name(rid) << OAS_ENDL;
}

void OasisWriter::write_magic()
{
    write_bytes(&OasisMagic[0], sizeof(OasisMagic));
}

MemoryOasisWriter::MemoryOasisWriter()
{
}

MemoryOasisWriter::MemoryOasisWriter(const OasisWriter& base_writer)
{
    set_option(base_writer.get_option());
    enable_cblock(base_writer.enable_cblock());
}

void MemoryOasisWriter::flush_buffer(bool atend)
{
    if(m_file_buffer.used() == 0)
    {
        return;
    }

    m_file_buffer.resize(2 * m_file_buffer.size());
}

const uint8* MemoryOasisWriter::get_data() const
{
    return m_file_buffer.data();
}

const uint32 MemoryOasisWriter::get_size() const
{
    return m_file_buffer.used();
}
