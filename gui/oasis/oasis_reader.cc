#include "oasis_reader.h"
#include "oasis_exception.h"
#include <iostream>

using namespace oasis;
using namespace std;

#define FILE_BUF_SIZE (1024 * 1024)
OasisReader::OasisReader() : m_byte_buffer(1024), m_file_buffer(FILE_BUF_SIZE), m_cblock_buffer(FILE_BUF_SIZE), m_in_cblock(false)
{
    m_decompressor = new OasisDecompressor;
    m_decompressor->initialize();
    m_file_offset = 0;
    m_file_buffer_size = FILE_BUF_SIZE;
}

OasisReader::~OasisReader()
{
    delete m_decompressor;
}

void OasisReader::init(FILE* fp, int64 offset)
{
    oas_assert(fp != NULL);
    m_fp = fp;
    seek(offset, true);
}

void OasisReader::reset_buf_size(uint64 size)
{
    uint64 round_size = 128;
    while (round_size < size && round_size < FILE_BUF_SIZE)
    {
        round_size <<= 1;
    }
    m_file_buffer_size = round_size;
}

void OasisReader::seek(int64 offset, bool force)
{
    if (offset < 0)
        offset += get_file_size();

    int64 bufferEndOffset = m_file_offset + (m_file_buffer.end - m_file_buffer.current);
    if (offset >= m_file_offset  &&  offset <= bufferEndOffset && !force)
    {
        m_file_buffer.current = m_file_buffer.begin + (offset - m_file_offset);
    }
    else
    {
        if (fseek(m_fp, offset, SEEK_SET) != 0)
        {
            OAS_DBG << "seek file failed" << OAS_ENDL;
            throw OasisException("seek file failed");
        }
        m_file_offset = offset;
        m_file_buffer.set_length(0);
    }

    leave_cblock();
    set_buffer(m_file_buffer);
}

int64 OasisReader::get_data_offset()
{
    if(!in_cblock())
    {
        return m_file_offset + (m_data_current - m_file_buffer.begin);
    }
    else
    {
        return m_file_offset + (m_file_buffer.current - m_file_buffer.begin);
    }
}

int64 OasisReader::get_file_size()
{
    int64 offset = ftell(m_fp);
    fseek(m_fp, 0, SEEK_SET);
    int64 start = ftell(m_fp);
    fseek(m_fp, 0, SEEK_END);
    int64 end = ftell(m_fp);
    fseek(m_fp, offset, SEEK_SET);
    return end - start;
}

void OasisReader::feed_buffer()
{
    oas_assert(buffer_empty());

    if(in_cblock())
    {
        get_buffer(m_cblock_buffer);
    }
    else
    {
        get_buffer(m_file_buffer);
    }

    if(in_cblock())
    {
        feed_cblock_buffer();

        if(in_cblock())
        {
            set_buffer(m_cblock_buffer);
        }
    }

    if(!in_cblock())
    {
        if(m_file_buffer.empty())
        {
            feed_file_buffer();
        }
        set_buffer(m_file_buffer);
    }

    oas_assert(!buffer_empty());
}

void OasisReader::feed_file_buffer()
{
    oas_assert(m_file_buffer.empty());

    int n = fread(m_file_buffer.begin, 1, m_file_buffer_size, m_fp);

    if(n == 0)
    {
        if(feof(m_fp))
        {
        }
        else
        {
            throw OasisException("read file failed");
        }
    }

    m_file_offset += (m_file_buffer.end - m_file_buffer.begin);
    m_file_buffer.set_length(n);
}

void OasisReader::feed_more_file_buffer(int size)
{
    int osize = m_file_buffer.end - m_file_buffer.begin;

    m_file_buffer.enlarge(size);

    int n = fread(m_file_buffer.begin + osize, 1, size, m_fp);

    if(n == 0)
    {
        if(feof(m_fp))
        {
            throw OasisException("unexpected eof");
        }
        else
        {
            throw OasisException("read file failed");
        }
    }
}

void OasisReader::feed_cblock_buffer()
{
    oas_assert(m_cblock_buffer.empty());

    if(m_uncompressed_bytes_left == 0 && m_compressed_bytes_left == 0)
    {
        leave_cblock();
        return;
    }

    if(m_compressed_bytes_left > 0)
    {
        if(m_file_buffer.empty())
        {
            feed_file_buffer();
        }

        uint64 availLen = m_file_buffer.data_length();
        if(availLen < m_compressed_bytes_left)
        {
            feed_more_file_buffer(m_compressed_bytes_left - availLen);
            availLen = m_file_buffer.data_length();
        }

        oas_assert(m_compressed_bytes_left <= availLen);

        int64 feedLen = m_compressed_bytes_left;
        m_decompressor->feed_all_data(m_file_buffer.current, feedLen);
        m_file_buffer.current += feedLen;
        m_compressed_bytes_left -= feedLen;
        oas_assert(m_compressed_bytes_left == 0);
    }

    uint32 fetchedSize;
    m_decompressor->fetch_data(m_cblock_buffer.begin, FILE_BUF_SIZE, fetchedSize);
    if(fetchedSize == 0)
    {
        if(m_uncompressed_bytes_left > 0 && m_compressed_bytes_left > 0)
        {
            OAS_DBG << "no suficient data, but we have fed all data" << OAS_ENDL;
            throw OasisException("no suficient data fed");
        }
        if(m_uncompressed_bytes_left > 0)
        {
            OAS_DBG << "too much uncompressed data " << m_uncompressed_bytes_left << OAS_ENDL;
            throw OasisException("too much uncompressed data");
        }
        if(m_compressed_bytes_left > 0)
        {
            OAS_DBG << "too much compressed data " << m_compressed_bytes_left << OAS_ENDL;
            throw OasisException("too much compressed data");
        }
        leave_cblock();
    }
    else if(fetchedSize > m_uncompressed_bytes_left)
    {
        throw OasisException("too much decompressed data");
    }
    else
    {
        m_cblock_buffer.set_length(fetchedSize);
        m_uncompressed_bytes_left -= fetchedSize;
    }
}

void OasisReader::enter_cblock(uint64 uncomp_byte_count, uint64 comp_byte_count)
{
    oas_assert(!in_cblock());
    m_uncompressed_bytes_left = uncomp_byte_count;
    m_compressed_bytes_left = comp_byte_count;

    if(m_uncompressed_bytes_left == 0)
    {
        skip_bytes(comp_byte_count);
        return;
    }

    oas_assert(comp_byte_count != 0);

    get_buffer(m_file_buffer);

    m_decompressor->prepare_decompress();

    m_cblock_buffer.set_length(0);
    set_buffer(m_cblock_buffer);
    m_in_cblock = true;
}


uint8 OasisReader::read_byte()
{
    if(buffer_empty())
    {
        feed_buffer();
    }
    return *m_data_current++;
}

uint8 OasisReader::peek_byte()
{
    if(buffer_empty())
    {
        feed_buffer();
    }
    return *m_data_current;
}

void OasisReader::read_bytes(uint8* buf, int32 len)
{
    while(len > 0)
    {
        if(buffer_empty())
        {
            feed_buffer();
        }
        int  n = std::min(len, available_data());
        memcpy(buf, m_data_current, n);
        buf = buf + n;
        m_data_current += n;
        len -= n;
    }
}

void OasisReader::skip_bytes(int32 len)
{
    while(len > 0)
    {
        if(buffer_empty())
        {
            feed_buffer();
        }
        int  n = std::min(len, available_data());
        m_data_current += n;
        len -= n;
    }
}

uint64 OasisReader::read_uint()
{
    int shift = 0;
    uint64 value = 0;
    while(1)
    {
        uint8 b = read_byte();
        uint8 more = b & 0x80;
        uint64 v = b & 0x7F;
        if(v) value += (v << shift);
        shift += 7;
        if(!more) break;
    }
    return value;
}

int64 OasisReader::read_int()
{
    uint64 v = read_uint();
    if((v & 1) != 0)
    {
        return -int64(v >> 1);
    }
    else
    {
        return int64(v >> 1);
    }
}

void OasisReader::skip_string()
{
    OasisString dummy_string;
    read_string(dummy_string);
}

void OasisReader::read_string(OasisString& ostring)
{
    uint64 len = read_uint();
    m_byte_buffer.Reserve(len);
    read_bytes(m_byte_buffer.Get(), len);
    ostring.m_value.assign(reinterpret_cast<char*>(m_byte_buffer.Get()), len);
    return;
}

void OasisReader::read_real(OasisReal& real)
{
    uint64 type = read_uint();
    oas_assert(type < OasisReal::ORT_Max);

    real.m_type = static_cast<OasisReal::Type>(type);
    switch(real.m_type)
    {
        case OasisReal::ORT_PositiveInteger:
            real.m_numerator = read_uint();
            real.m_denominator = 1;
            real.m_value = (float64)real.m_numerator;
            break;
        case OasisReal::ORT_NegativeInteger:
            real.m_numerator = read_uint();
            real.m_denominator = 1;
            real.m_value = -(float64)real.m_numerator;
            break;
        case OasisReal::ORT_PositiveReciprocal:
            real.m_numerator = 1;
            real.m_denominator = read_uint();
            real.m_value = 1.0 / real.m_denominator;
            break;
        case OasisReal::ORT_NegativeReciprocal:
            real.m_numerator = 1;
            real.m_denominator = read_uint();
            real.m_value = -1.0 / real.m_denominator;
            break;
        case OasisReal::ORT_PositiveRatio:
            real.m_numerator = read_uint();
            real.m_denominator = read_uint();
            real.m_value = (float64)real.m_numerator / real.m_denominator;
            break;
        case OasisReal::ORT_NegativeRatio:
            real.m_numerator = read_uint();
            real.m_denominator = read_uint();
            real.m_value = -(float64)real.m_numerator / real.m_denominator;
            break;
        case OasisReal::ORT_Float32:
            {
            float32 value;
            read_bytes((uint8*)&value, sizeof(value));
            real.m_value = value;
            }
            break;
        case OasisReal::ORT_Float64:
            {
            float64 value;
            read_bytes((uint8*)&value, sizeof(value));
            real.m_value = value;
            }
            break;
        default:
            break;
    }
    return;
}

void OasisReader::read_delta_1(OasisDelta& delta, bool horizontal)
{
    delta.m_type = OasisDelta::ODT_Delta1;
    if(horizontal)
    {
        delta.m_dir = OasisDelta::Horizontal;
        delta.m_dx = read_int();
        delta.m_dy = 0;
    }
    else
    {
        delta.m_dir = OasisDelta::Vertical;
        delta.m_dx = 0;
        delta.m_dy = read_int();
    }
    return;
}

void OasisReader::read_delta_2(OasisDelta& delta)
{
    delta.m_type = OasisDelta::ODT_Delta2;
    uint64 value = read_uint();
    OasisDelta::Direction dir = static_cast<OasisDelta::Direction>(value & 0x03);
    value >>= 2;
    delta.setValue(dir, value);
    return;
}

void OasisReader::read_delta_3(OasisDelta& delta)
{
    delta.m_type = OasisDelta::ODT_Delta3;
    uint64 value = read_uint();
    OasisDelta::Direction dir = static_cast<OasisDelta::Direction>(value & 0x07);
    value >>= 3;
    delta.setValue(dir, value);
    return;
}

void OasisReader::read_delta_g(OasisDelta& delta)
{
    delta.m_type = OasisDelta::ODT_DeltaG;
    uint64 value = read_uint();
    bool delta3 = !(value & 0x01);
    value >>= 1;
    if(delta3)
    {
        OasisDelta::Direction dir = static_cast<OasisDelta::Direction>(value & 0x07);
        value >>= 3;
        delta.setValue(dir, value);
        return;
    }

    delta.m_dir = OasisDelta::Any;
    delta.m_dx = uint_to_int(value);
    delta.m_dy = read_int();
    return;
}

void OasisReader::read_repetition(OasisRepetition& repetition)
{
    uint64 type = read_uint();
    oas_assert(type < OasisRepetition::ORT_Max);

    if(type == OasisRepetition::ORT_ReusePrevious)
    {
        return;
    }
    repetition.reset();
    repetition.m_type = static_cast<OasisRepetition::Type>(type);
    switch(repetition.m_type)
    {
        case OasisRepetition::ORT_ReusePrevious:
            oas_assert(false);
            break;
        case OasisRepetition::ORT_Regular2D:
            repetition.m_dimensionX = read_uint();
            repetition.m_dimensionY = read_uint();
            repetition.m_spaces.resize(2);
            repetition.m_spaces[0] = read_uint();
            repetition.m_spaces[1] = read_uint();

            oas_assert(repetition.m_spaces[0] > 0 || repetition.m_spaces[1] > 0);
            if (repetition.m_spaces[0] == 0)
            {
                repetition.m_type = OasisRepetition::ORT_VerticalRegular;
                repetition.m_spaces[0] = repetition.m_spaces[1];
                repetition.m_spaces.resize(1);
            }
            else if (repetition.m_spaces[1] == 0)
            {
                repetition.m_type = OasisRepetition::ORT_HorizontalRegular;
                repetition.m_spaces.resize(1);
            }
            break;
        case OasisRepetition::ORT_HorizontalRegular:
            repetition.m_dimensionX = read_uint();
            repetition.m_spaces.resize(1);
            repetition.m_spaces[0] = read_uint();
            break;
        case OasisRepetition::ORT_VerticalRegular:
            repetition.m_dimensionY = read_uint();
            repetition.m_spaces.resize(1);
            repetition.m_spaces[0] = read_uint();
            break;
        case OasisRepetition::ORT_HorizontalIrregular:
            repetition.m_dimensionX = read_uint();
            repetition.m_spaces.resize(repetition.m_dimensionX + 1);
            for(uint32 i = 0; i < repetition.m_spaces.size(); ++i)
            {
                repetition.m_spaces[i] = read_uint();
            }
            break;
        case OasisRepetition::ORT_HorizontalIrregularGrid:
            repetition.m_dimensionX = read_uint();
            repetition.m_grid = read_uint();
            repetition.m_spaces.resize(repetition.m_dimensionX + 1);
            for(uint32 i = 0; i < repetition.m_spaces.size(); ++i)
            {
                repetition.m_spaces[i] = read_uint();
            }
            break;
        case OasisRepetition::ORT_VerticalIrregular:
            repetition.m_dimensionY = read_uint();
            repetition.m_spaces.resize(repetition.m_dimensionY + 1);
            for(uint32 i = 0; i < repetition.m_spaces.size(); ++i)
            {
                repetition.m_spaces[i] = read_uint();
            }
            break;
        case OasisRepetition::ORT_VerticalIrregularGrid:
            repetition.m_dimensionY = read_uint();
            repetition.m_grid = read_uint();
            repetition.m_spaces.resize(repetition.m_dimensionY + 1);
            for(uint32 i = 0; i < repetition.m_spaces.size(); ++i)
            {
                repetition.m_spaces[i] = read_uint();
            }
            break;
        case OasisRepetition::ORT_DiagonalRegular2D:
            repetition.m_dimensionN = read_uint();
            repetition.m_dimensionM = read_uint();
            repetition.m_deltas.resize(2);
            read_delta_g(repetition.m_deltas[0]);
            read_delta_g(repetition.m_deltas[1]);
            break;
        case OasisRepetition::ORT_DiagonalRegular1D:
            repetition.m_dimension = read_uint();
            repetition.m_deltas.resize(1);
            read_delta_g(repetition.m_deltas[0]);
            break;
        case OasisRepetition::ORT_Arbitory:
            repetition.m_dimension = read_uint();
            repetition.m_deltas.resize(repetition.m_dimension + 1);
            for(uint32 i = 0; i < repetition.m_deltas.size(); ++i)
            {
                read_delta_g(repetition.m_deltas[i]);
            }
            break;
        case OasisRepetition::ORT_ArbitoryGrid:
            repetition.m_dimension = read_uint();
            repetition.m_grid = read_uint();
            repetition.m_deltas.resize(repetition.m_dimension + 1);
            for(uint32 i = 0; i < repetition.m_deltas.size(); ++i)
            {
                read_delta_g(repetition.m_deltas[i]);
            }
            break;
        default:
            break;
    }
    return;
}

void OasisReader::read_point_list(OasisPointList& pointlist)
{
    uint64 type = read_uint();
    oas_assert(type < OasisPointList::OPLT_Max);
    pointlist.m_type = static_cast<OasisPointList::Type>(type);

    uint64 count = read_uint();
    pointlist.m_deltas.resize(count);
    bool horizontal = false;
    switch(pointlist.m_type)
    {
        case OasisPointList::OPLT_ManhattanHorizontalFirst:
            horizontal = true;
        case OasisPointList::OPLT_ManhattanVerticalFirst:
            for(uint64 i = 0; i < count; ++i)
            {
                read_delta_1(pointlist.m_deltas[i], horizontal);
                horizontal = !horizontal;
            }
            break;
        case OasisPointList::OPLT_Manhattan:
            for(uint64 i = 0; i < count; ++i)
            {
                read_delta_2(pointlist.m_deltas[i]);
            }
            break;
        case OasisPointList::OPLT_Octangular:
            for(uint64 i = 0; i < count; ++i)
            {
                read_delta_3(pointlist.m_deltas[i]);
            }
            break;
        case OasisPointList::OPLT_AnyAngle:
        case OasisPointList::OPLT_AnyAngleAdjust:
            for(uint64 i = 0; i < count; ++i)
            {
                read_delta_g(pointlist.m_deltas[i]);
            }
            break;
        default:
            break;
    }
    return;
}

void OasisReader::read_prop_value(OasisPropValue& propvalue)
{
    uint64 type = peek_byte();
    oas_assert(type < OasisPropValue::OPVT_Max);
    propvalue.m_type = static_cast<OasisPropValue::Type>(type);

    if(type < OasisReal::ORT_Max)
    {
        read_real(propvalue.m_real_value);
        return;
    }

    type = read_uint();
    switch(propvalue.m_type)
    {
        case OasisPropValue::OPVT_UnsignedInteger:
            propvalue.m_uint_value = read_uint();
            break;
        case OasisPropValue::OPVT_SignedInteger:
            propvalue.m_int_value = read_int();
            break;
        case OasisPropValue::OPVT_AString:
            read_string(propvalue.m_string_value);
            oas_assert(propvalue.m_string_value.is_astring());
            break;
        case OasisPropValue::OPVT_BString:
            read_string(propvalue.m_string_value);
            oas_assert(propvalue.m_string_value.is_bstring());
            break;
        case OasisPropValue::OPVT_NString:
            read_string(propvalue.m_string_value);
            oas_assert(propvalue.m_string_value.is_nstring());
            break;
        case OasisPropValue::OPVT_RefAString:
        case OasisPropValue::OPVT_RefBString:
        case OasisPropValue::OPVT_RefNString:
            propvalue.m_ref_value = read_uint();
            break;
        default:
            break;
    }
    return;
}

void OasisReader::read_interval(OasisInterval& interval)
{
    uint8 type = read_uint();
    oas_assert(type < OasisInterval::OIT_Max);
    interval.m_type = static_cast<OasisInterval::Type>(type);

    switch(interval.m_type)
    {
        case OasisInterval::OIT_All:
            interval.m_lower = 0;
            interval.m_upper = OasisInterval::Infinite;
            break;
        case OasisInterval::OIT_BoundBellow:
            interval.m_lower = 0;
            interval.m_upper = read_uint();
            break;
        case OasisInterval::OIT_BoundAbove:
            interval.m_lower = read_uint();
            interval.m_upper = OasisInterval::Infinite;
            break;
        case OasisInterval::OIT_Exact:
            interval.m_lower = interval.m_upper = read_uint();
            break;
        case OasisInterval::OIT_BoundAll:
            interval.m_lower = read_uint();
            interval.m_upper = read_uint();
            break;
        default:
            break;
    }
}


