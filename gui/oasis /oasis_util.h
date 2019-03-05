#ifndef OASIS_UTIL_H
#define OASIS_UTIL_H
#include "oasis_types.h"
#include <vector>
#include <set>
#include <utility>
#include <string.h>

namespace oasis
{
const static std::string generate_cellname_prefix = "#pgcn#_";

class ReadBuffer
{
public:
    uint8* current;
    uint8* end;
    uint8* begin;

    ReadBuffer() : current(NULL), end(NULL), begin(NULL) {}
    ReadBuffer(int bufsize)
    {
        begin = new uint8[bufsize];
        current = end = begin;
    }
    ~ReadBuffer()
    {
        delete [] begin;
    }
    void set_length(int len)
    {
        oas_assert(begin != NULL);
        current = begin;
        end = begin + len;
    }
    void enlarge(int size)
    {
        oas_assert(begin != NULL);
        int32 currlen = current - begin;
        int32 oldsize = end - begin;
        uint8 *newbuf = new uint8[size + oldsize];
        memcpy(newbuf, begin, oldsize * sizeof(uint8));
        delete [] begin;
        begin = newbuf;
        current = newbuf + currlen;
        end = begin + oldsize + size;
    }
    int32 data_length() const
    {
        oas_assert(begin != NULL);
        return end - current;
    }
    bool empty() const
    {
        oas_assert(begin != NULL);
        return end == current;
    }
private:
    ReadBuffer(const ReadBuffer&);
    ReadBuffer& operator=(const ReadBuffer&);
};

class OasisPointListBuilder
{
public:
    OasisPointListBuilder(bool ispoly = true) : m_is_polygon(ispoly) {}

    bool points_to_list(const std::vector<Point>& points, OasisPointList& pointlist);
    bool list_to_points(const OasisPointList& pointlist, std::vector<Point>& points);
private:
    bool check_type(const std::vector<Point>& points);
    bool calculate_direction_and_distance(const Point& a, const Point& b, OasisDelta::Direction& dir, uint64& dist);

    bool m_is_polygon;
    OasisPointList::Type m_type;
};

struct FileProperty
{
    FileProperty() : m_major_version(0), m_minor_version(0), m_bounding_boxes_available(0)
    {
       for (uint32 i = 0; i < 5; ++i) m_orginalfile_sha1[i] = 0;
    }

    void set_origin_file_sha1(const uint32 sourcefile_sha1_[])
    {
       for (uint32 i = 0; i < 5; ++i) m_orginalfile_sha1[i] = sourcefile_sha1_[i];
    }

    void get_origin_file_sha1(uint32 sourcefile_sha1_[])
    {
       for (uint32 i = 0; i < 5; ++i) sourcefile_sha1_[i] = m_orginalfile_sha1[i];
    }

    uint64 m_major_version;
    uint64 m_minor_version;
    uint32 m_orginalfile_sha1[5];

    std::vector<std::string> m_top_cells;
    uint32 m_bounding_boxes_available;
};
struct CellProperty
{
    CellProperty() : m_offset(0), m_cellsize(4096), m_is_topcell(false) {}
    Box m_bbox;
    uint64 m_offset;
    uint64 m_cellsize;
    bool m_is_topcell;
    std::set<std::pair<int32, int32> > m_layers;
};

class OasisProperty;
class OasisLayout;
class OasisPropertyBuilder
{
public:
    void encode_file_properties(OasisLayout &layout, const FileProperty &fileprop, std::vector<OasisProperty> &properties);
    void decode_file_properties(OasisLayout &layout, FileProperty &fileprop, const std::vector<OasisProperty> &properties);

    void encode_cell_properties(OasisLayout &layout, const CellProperty &cellprop, std::vector<OasisProperty> &properties);
    void decode_file_properties(OasisLayout &layout, CellProperty &cellprop, const std::vector<OasisProperty> &properties);
};

class OasisRepetitionBuilder
{
public:
    OasisRepetitionBuilder(bool do_partition = true) { m_do_partition = do_partition; }
    bool partition(const OasisRepetition& repetition, int64 limit, std::vector<OasisRepetition>& results, std::vector<Point>& offsets);
    void make_vertical_regular(OasisRepetition& repetition, uint64 count, int64 space);
    void make_horizontal_regular(OasisRepetition& repetition, uint64 count, int64 space);
    void make_regular_2d(OasisRepetition& repetition, uint64 countx, int64 spacex, uint64 county, int64 spacey);
    void make_diagonal_1d(OasisRepetition& repetition, uint64 count, const OasisDelta& delta);
    void make_diagonal_2d(OasisRepetition& repetition, uint64 countn, const OasisDelta& deltan, uint64 countm, const OasisDelta& deltam);
    void make_arbitory(OasisRepetition& repetition, const std::vector<Point>& disps, uint32 begin, uint32 end);
    void make_vertical_irregular(OasisRepetition& repetition, const std::vector<Point>& disps, uint32 begin, uint32 end);
    void make_horizontal_irregular(OasisRepetition& repetition, const std::vector<Point>& disps, uint32 begin, uint32 end);
    bool m_do_partition;
};

class OasisIntervalBuilder
{
public:
    OasisInterval make_interval(OasisInterval::Type type, uint32 lower, uint32 upper);
};

class FormatDetector
{
public:
    static std::string detect_format(const std::string& filename);
};
}
#endif
