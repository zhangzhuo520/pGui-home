#include "oasis_util.h"
#include "oasis_layout.h"
#include "oasis_cell.h"
#include "oasis_parser.h"
#include "gds_parser.h"
#include "oasis_exception.h"

using namespace oasis;

bool OasisPointListBuilder::check_type(const std::vector<Point>& points)
{
    if (points.size() <= 1 + (m_is_polygon ? 1 : 0))
    {
        return false;
    }

    bool prev_horizontal = true;
    Point e = points[1] - points[0];
    if(e.y() == 0)
    {
        m_type = OasisPointList::OPLT_ManhattanHorizontalFirst;
        prev_horizontal = true;
    }
    else if(e.x() == 0)
    {
        m_type = OasisPointList::OPLT_ManhattanVerticalFirst;
        prev_horizontal = false;
    }
    else if(e.x() == e.y() || e.x() == -e.y())
    {
        m_type = OasisPointList::OPLT_Octangular;
    }
    else
    {
        m_type = OasisPointList::OPLT_AnyAngle;
        return true;
    }
    for(uint32 i = 1; i < points.size(); ++i)
    {
        uint32 j = i + 1;
        if(j == points.size())
        {
            if(!m_is_polygon)
            {
                break;
            }
            j = 0;
        }

        e = points[j] - points[i];

        if(m_type == OasisPointList::OPLT_Octangular)
        {
            if(e.x() == 0 || e.y() == 0 || e.x() == e.y() || e.x() == -e.y())
            {
            }
            else
            {
                m_type = OasisPointList::OPLT_AnyAngle;
                return true;
            }
        }
        else if(m_type == OasisPointList::OPLT_Manhattan)
        {
            if(e.x() == 0 || e.y() == 0)
            {
            }
            else if(e.x() == e.y() || e.x() == -e.y())
            {
                m_type = OasisPointList::OPLT_Octangular;
            }
            else
            {
                m_type = OasisPointList::OPLT_AnyAngle;
                return true;
            }
        }
        else if(m_type == OasisPointList::OPLT_ManhattanVerticalFirst || m_type == OasisPointList::OPLT_ManhattanHorizontalFirst)
        {
            if(e.x() == 0)
            {
                if(prev_horizontal)
                {
                    prev_horizontal = !prev_horizontal;
                }
                else
                {
                    m_type = OasisPointList::OPLT_Manhattan;
                }
            }
            else if(e.y() == 0)
            {
                if(!prev_horizontal)
                {
                    prev_horizontal = !prev_horizontal;
                }
                else
                {
                    m_type = OasisPointList::OPLT_Manhattan;
                }
            }
            else if(e.x() == e.y() || e.x() == -e.y())
            {
                m_type = OasisPointList::OPLT_Octangular;
            }
            else
            {
                m_type = OasisPointList::OPLT_AnyAngle;
                return true;
            }
        }
    }

    if(m_type == OasisPointList::OPLT_ManhattanHorizontalFirst || m_type == OasisPointList::OPLT_ManhattanVerticalFirst)
    {
        if(m_is_polygon && (points.size() % 2) != 0)
        {
            m_type = OasisPointList::OPLT_Manhattan;
        }
    }
    return true;
}

bool OasisPointListBuilder::calculate_direction_and_distance(const Point& a, const Point& b, OasisDelta::Direction& dir, uint64& dist)
{
    int64 dx = b.x() - a.x();
    int64 dy = b.y() - a.y();
    if(dx > 0)
    {
        if(dy > 0)
        {
            dir = OasisDelta::NorthEast;
        }
        else if(dy < 0)
        {
            dir = OasisDelta::SouthEast;
        }
        else
        {
            dir = OasisDelta::East;
        }
        dist = dx;
    }
    else if(dx < 0 )
    {
        if(dy > 0)
        {
            dir = OasisDelta::NorthWest;
        }
        else if(dy < 0)
        {
            dir = OasisDelta::SouthWest;
        }
        else
        {
            dir = OasisDelta::West;
        }
        dist = -dx;
    }
    else
    {
        if(dy > 0)
        {
            dir = OasisDelta::North;
            dist = dy;
        }
        else if(dy < 0)
        {
            dir = OasisDelta::South;
            dist = -dy;
        }
        else
        {
            dir = OasisDelta::East;
            dist = 0;
            return false;
        }
    }
    return true;
}

bool OasisPointListBuilder::points_to_list(const std::vector<Point>& points, OasisPointList& pointlist)
{
    if (!check_type(points))
    {
        return false;
    }

    if(m_type == OasisPointList::OPLT_ManhattanHorizontalFirst || m_type == OasisPointList::OPLT_ManhattanVerticalFirst)
    {
        pointlist.m_type = m_type;
        pointlist.m_deltas.clear();
        uint32 sz = points.size() - 1 - (m_is_polygon ? 1 : 0);
        pointlist.m_deltas.reserve(sz);
        bool horizontal = (m_type == OasisPointList::OPLT_ManhattanHorizontalFirst);
        for(uint32 i = 1; i <= sz; ++i)
        {
            OasisDelta d;
            d.m_type = OasisDelta::ODT_Delta1;
            if(horizontal)
            {
                d.m_dir = OasisDelta::Horizontal;
                d.m_dx = points[i].x() - points[i - 1].x();
                d.m_dy = 0;
            }
            else
            {
                d.m_dir = OasisDelta::Vertical;
                d.m_dx = 0;
                d.m_dy = points[i].y() - points[i - 1].y();
            }
            pointlist.m_deltas.push_back(d);
            horizontal = !horizontal;
        }
    }
    else if(m_type == OasisPointList::OPLT_Manhattan)
    {
        pointlist.m_type = m_type;
        pointlist.m_deltas.clear();
        uint32 sz = points.size() - 1;
        pointlist.m_deltas.reserve(sz);
        for(uint32 i = 1; i <= sz; ++i)
        {
            OasisDelta d;
            d.m_type = OasisDelta::ODT_Delta2;
            OasisDelta::Direction dir;
            uint64 dist;
            if (!calculate_direction_and_distance(points[i - 1], points[i], dir, dist))
            {
                return false;
            }
            d.setValue(dir, dist);
            pointlist.m_deltas.push_back(d);
        }
    }
    else if(m_type == OasisPointList::OPLT_Octangular)
    {
        pointlist.m_type = m_type;
        pointlist.m_deltas.clear();
        uint32 sz = points.size() - 1;
        pointlist.m_deltas.reserve(sz);
        for(uint32 i = 1; i <= sz; ++i)
        {
            OasisDelta d;
            d.m_type = OasisDelta::ODT_Delta3;
            OasisDelta::Direction dir;
            uint64 dist;
            if (!calculate_direction_and_distance(points[i - 1], points[i], dir, dist))
            {
                return false;
            }
            d.setValue(dir, dist);
            pointlist.m_deltas.push_back(d);
        }
    }
    else if(m_type == OasisPointList::OPLT_AnyAngle)
    {
        pointlist.m_type = m_type;
        pointlist.m_deltas.clear();
        uint32 sz = points.size() - 1;
        pointlist.m_deltas.reserve(sz);
        for(uint32 i = 1; i <= sz; ++i)
        {
            OasisDelta d;
            d.m_type = OasisDelta::ODT_DeltaG;
            d.m_dir = OasisDelta::Any;
            d.m_dx = points[i].x() - points[i - 1].x();
            d.m_dy = points[i].y() - points[i - 1].y();
            pointlist.m_deltas.push_back(d);
        }
    }
    else
    {
        return false;
    }
    return true;
}


bool OasisPointListBuilder::list_to_points(const OasisPointList& pointlist, std::vector<Point>& points)
{
    m_type = pointlist.m_type;
    if(m_type == OasisPointList::OPLT_ManhattanHorizontalFirst || m_type == OasisPointList::OPLT_ManhattanVerticalFirst)
    {
        points.clear();
        uint32 sz = pointlist.m_deltas.size() + 1 + (m_is_polygon ? 1 : 0);
        points.reserve(sz);
        bool horizontal = (m_type == OasisPointList::OPLT_ManhattanHorizontalFirst);
        Point p;
        points.push_back(p);
        for(uint32 i = 0; i < pointlist.m_deltas.size(); ++i)
        {
            const OasisDelta& d = pointlist.m_deltas[i];
            if(horizontal)
            {
                p.set_x(p.x() + d.dx());
            }
            else
            {
                p.set_y(p.y() + d.dy());
            }
            points.push_back(p);
            horizontal = !horizontal;
        }
        if(m_is_polygon)
        {
            if(m_type == OasisPointList::OPLT_ManhattanHorizontalFirst)
            {
                p.set_x(0);
            }
            else
            {
                p.set_y(0);
            }
            points.push_back(p);
        }
    }
    else if(m_type == OasisPointList::OPLT_Manhattan ||
            m_type == OasisPointList::OPLT_Octangular ||
            m_type == OasisPointList::OPLT_AnyAngle)
    {
        points.clear();
        uint32 sz = pointlist.m_deltas.size() + 1;
        points.reserve(sz);
        Point p;
        points.push_back(p);
        for(uint32 i = 0; i < pointlist.m_deltas.size(); ++i)
        {
            const OasisDelta& d = pointlist.m_deltas[i];
            p.set_x(p.x() + d.dx());
            p.set_y(p.y() + d.dy());
            points.push_back(p);
        }
        if (OasisComply::comply())
        {
            if(m_is_polygon && m_type == OasisPointList::OPLT_Manhattan)
            {
                if (!(p.x() == 0 || p.y() == 0))
                {
                    return false;
                }
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

void OasisPropertyBuilder::encode_file_properties(OasisLayout &layout, const FileProperty &fileprop, std::vector<OasisProperty> &properties)
{
    properties.clear();
    OasisProperty property;

    for (size_t i = 0; i < fileprop.m_top_cells.size(); ++i)
    {
        int64 propname_index = layout.add_prop_name("S_TOP_CELL");

        property.m_is_standard = true;
        property.m_prop_name.set_refnum(propname_index);
        property.m_prop_name.set_index(propname_index);
        property.m_prop_values.clear();

        int64 propstring_index = layout.add_prop_string(fileprop.m_top_cells[i]);

        OasisPropValue topcell;
        topcell.m_type = OasisPropValue::OPVT_RefNString;
        topcell.m_ref_value = propstring_index;
        property.m_prop_values.push_back(topcell);
        properties.push_back(property);
    }

    if (fileprop.m_bounding_boxes_available > 0)
    {
        int64 propname_index = layout.add_prop_name("S_BOUNDING_BOXES_AVAILABLE");

        property.m_is_standard = true;
        property.m_prop_name.set_refnum(propname_index);
        property.m_prop_name.set_index(propname_index);
        property.m_prop_values.clear();
        OasisPropValue bbox_available;
        bbox_available.m_type = OasisPropValue::OPVT_UnsignedInteger;
        bbox_available.m_uint_value = fileprop.m_bounding_boxes_available;
        property.m_prop_values.push_back(bbox_available);
        properties.push_back(property);
    }

    int64 propname_index = layout.add_prop_name("PDB_FILE_VERSION");

    property.m_is_standard = false;
    property.m_prop_name.set_refnum(propname_index);
    property.m_prop_name.set_index(propname_index);
    property.m_prop_values.clear();
    OasisPropValue major;
    major.m_type = OasisPropValue::OPVT_UnsignedInteger;
    major.m_uint_value = fileprop.m_major_version;
    property.m_prop_values.push_back(major);
    OasisPropValue minor;
    minor.m_type = OasisPropValue::OPVT_UnsignedInteger;
    minor.m_uint_value = fileprop.m_minor_version;
    property.m_prop_values.push_back(minor);

    for (uint32 i = 0; i < 5; ++i)
    {
       OasisPropValue sha1_byte;
       sha1_byte.m_type = OasisPropValue::OPVT_UnsignedInteger;
       sha1_byte.m_uint_value = fileprop.m_orginalfile_sha1[i];
       property.m_prop_values.push_back(sha1_byte);
    }
    properties.push_back(property);
}

void OasisPropertyBuilder::decode_file_properties(OasisLayout& layout, FileProperty& fileprop, const std::vector<OasisProperty>& properties)
{
    bool is_standard = true;
    for (size_t i = 0; i < properties.size(); ++i)
    {
        const OasisProperty &property = properties[i];

        if (property.m_is_standard && !is_standard)
        {
            throw OasisException("file standard property is after non standard ones");
        }
        is_standard = property.m_is_standard;

        oas_assert(property.m_prop_name.is_referenced() || property.m_prop_name.is_named());
        std::string name;
        if (property.m_prop_name.is_referenced())
        {
            const OasisPropName &propname = layout.get_prop_name(property.m_prop_name.get_refnum());
            name = propname.get_name().value();
        }
        else
        {
            name = property.m_prop_name.get_name().value();
        }
        if (name == std::string("S_TOP_CELL"))
        {
            oas_assert(property.m_is_standard == true);
            oas_assert(property.m_prop_values.size() == 1);
            const OasisPropValue& prop_value = property.m_prop_values[0];
            oas_assert(prop_value.m_type == OasisPropValue::OPVT_RefNString || prop_value.m_type == OasisPropValue::OPVT_NString);
            if (prop_value.m_type == OasisPropValue::OPVT_NString)
            {
                fileprop.m_top_cells.push_back(prop_value.m_string_value.value());
            }
            else
            {
                const OasisPropString& prop_string = layout.get_prop_string(prop_value.m_ref_value);
                fileprop.m_top_cells.push_back(prop_string.get_name().value());
            }
        }
        else if (name == std::string("S_BOUNDING_BOXES_AVAILABLE"))
        {
            oas_assert(property.m_is_standard == true);
            oas_assert(property.m_prop_values.size() == 1);
            const OasisPropValue &prop_value = property.m_prop_values[0];
            oas_assert(prop_value.m_type == OasisPropValue::OPVT_UnsignedInteger);
            fileprop.m_bounding_boxes_available = prop_value.m_uint_value;
        }
        else if (name == std::string("PDB_FILE_VERSION"))
        {
            oas_assert(property.m_is_standard == false);
            oas_assert(property.m_prop_values.size() >= 2);
            const OasisPropValue& major = property.m_prop_values[0];
            const OasisPropValue& minor = property.m_prop_values[1];
            oas_assert(major.m_type == OasisPropValue::OPVT_UnsignedInteger);
            fileprop.m_major_version = major.m_uint_value;
            oas_assert(minor.m_type == OasisPropValue::OPVT_UnsignedInteger);
            fileprop.m_minor_version = minor.m_uint_value;

            uint32 index = 2;
            if (property.m_prop_values.size() >= index + 5)
            {
               for (uint32 i = 0; i < 5; ++index, ++i)
               {
                  const OasisPropValue& sha1_byte = property.m_prop_values[index];
                  oas_assert(sha1_byte.m_type == OasisPropValue::OPVT_UnsignedInteger);
                  fileprop.m_orginalfile_sha1[i] = (uint32)sha1_byte.m_uint_value;
               }
            }

        }
    }
}

void OasisPropertyBuilder::encode_cell_properties(OasisLayout& layout, const CellProperty &cellprop, std::vector<OasisProperty> &properties)
{
    int64 propname_index = layout.add_prop_name("S_BOUNDING_BOX");
    OasisProperty bounding_box;
    bounding_box.m_is_standard = true;
    bounding_box.m_prop_name.set_refnum(propname_index);
    bounding_box.m_prop_name.set_index(propname_index);
    bounding_box.m_prop_values.clear();

    OasisPropValue flags;
    flags.m_type = OasisPropValue::OPVT_UnsignedInteger;
    flags.m_uint_value = 0;
    bounding_box.m_prop_values.push_back(flags);

    OasisPropValue left;
    left.m_type = OasisPropValue::OPVT_SignedInteger;
    left.m_int_value = cellprop.m_bbox.left();
    bounding_box.m_prop_values.push_back(left);

    OasisPropValue bottom;
    bottom.m_type = OasisPropValue::OPVT_SignedInteger;
    bottom.m_int_value = cellprop.m_bbox.bottom();
    bounding_box.m_prop_values.push_back(bottom);

    OasisPropValue width;
    width.m_type = OasisPropValue::OPVT_UnsignedInteger;
    width.m_uint_value = cellprop.m_bbox.width();
    bounding_box.m_prop_values.push_back(width);

    OasisPropValue height;
    height.m_type = OasisPropValue::OPVT_UnsignedInteger;
    height.m_uint_value = cellprop.m_bbox.height();
    bounding_box.m_prop_values.push_back(height);

    properties.push_back(bounding_box);

    propname_index = layout.add_prop_name("S_CELL_OFFSET");
    OasisProperty offset;
    offset.m_is_standard = true;
    offset.m_prop_name.set_refnum(propname_index);
    offset.m_prop_name.set_index(propname_index);
    offset.m_prop_values.clear();

    OasisPropValue offset_value;
    offset_value.m_type = OasisPropValue::OPVT_UnsignedInteger;
    offset_value.m_uint_value = cellprop.m_offset;
    offset.m_prop_values.push_back(offset_value);

    properties.push_back(offset);

    propname_index = layout.add_prop_name("PDB_CELL_SIZE");
    OasisProperty cellsize;
    cellsize.m_is_standard = false;
    cellsize.m_prop_name.set_refnum(propname_index);
    cellsize.m_prop_name.set_index(propname_index);
    cellsize.m_prop_values.clear();

    OasisPropValue cellsize_value;
    cellsize_value.m_type = OasisPropValue::OPVT_UnsignedInteger;
    cellsize_value.m_uint_value = cellprop.m_cellsize;
    cellsize.m_prop_values.push_back(cellsize_value);

    properties.push_back(cellsize);

    if (cellprop.m_is_topcell)
    {
        propname_index = layout.add_prop_name("PDB_IS_TOPCELL");
        OasisProperty topcell;
        topcell.m_is_standard = false;
        topcell.m_prop_name.set_refnum(propname_index);
        topcell.m_prop_name.set_index(propname_index);
        topcell.m_prop_values.clear();

        OasisPropValue istopcell;
        istopcell.m_type = OasisPropValue::OPVT_UnsignedInteger;
        istopcell.m_uint_value = 1;
        topcell.m_prop_values.push_back(istopcell);

        properties.push_back(topcell);
    }

    if (!cellprop.m_layers.empty())
    {
        propname_index = layout.add_prop_name("PDB_LAYERS");
        OasisProperty layers;
        layers.m_is_standard = false;
        layers.m_prop_name.set_refnum(propname_index);
        layers.m_prop_name.set_index(propname_index);
        layers.m_prop_values.clear();

        OasisPropValue lcount;
        lcount.m_type = OasisPropValue::OPVT_UnsignedInteger;
        lcount.m_uint_value = cellprop.m_layers.size();
        layers.m_prop_values.push_back(lcount);

        for (auto it = cellprop.m_layers.begin(); it != cellprop.m_layers.end(); ++it)
        {
            OasisPropValue layer;
            layer.m_type = OasisPropValue::OPVT_UnsignedInteger;
            layer.m_uint_value = it->first;
            layers.m_prop_values.push_back(layer);
            OasisPropValue datatype;
            datatype.m_type = OasisPropValue::OPVT_UnsignedInteger;
            datatype.m_uint_value = it->second;
            layers.m_prop_values.push_back(datatype);
        }
        properties.push_back(layers);
    }
}

void OasisPropertyBuilder::decode_file_properties(OasisLayout& layout, CellProperty &cellprop, const std::vector<OasisProperty> &properties)
{
    bool is_prev_standard = true;
    for (size_t i = 0; i < properties.size(); ++i)
    {
        const OasisProperty &property = properties[i];
        if (property.m_is_standard && !is_prev_standard)
        {
            throw OasisException("cell standard property is after non standard propery");
        }
        is_prev_standard = property.m_is_standard;

        oas_assert(property.m_prop_name.is_referenced() || property.m_prop_name.is_named());

        std::string name;
        if (property.m_prop_name.is_referenced())
        {
            const OasisPropName &propname = layout.get_prop_name(property.m_prop_name.get_refnum());
            name = propname.get_name().value();
        }
        else
        {
            name = property.m_prop_name.get_name().value();
        }
        if (name == "S_BOUNDING_BOX")
        {
            oas_assert(property.m_is_standard == true);
            oas_assert(property.m_prop_values.size() == 5);

            const OasisPropValue& flags = property.m_prop_values[0];
            oas_assert(flags.m_type == OasisPropValue::OPVT_UnsignedInteger);
            oas_assert(flags.m_uint_value == 0);

            const OasisPropValue& left = property.m_prop_values[1];
            oas_assert(left.m_type == OasisPropValue::OPVT_SignedInteger);
            cellprop.m_bbox.set_left(left.m_int_value);

            const OasisPropValue& bottom = property.m_prop_values[2];
            oas_assert(bottom.m_type == OasisPropValue::OPVT_SignedInteger);
            cellprop.m_bbox.set_bottom(bottom.m_int_value);

            const OasisPropValue& width = property.m_prop_values[3];
            oas_assert(width.m_type == OasisPropValue::OPVT_UnsignedInteger);
            cellprop.m_bbox.set_right(left.m_int_value + width.m_uint_value);

            const OasisPropValue& height = property.m_prop_values[4];
            oas_assert(height.m_type == OasisPropValue::OPVT_UnsignedInteger);
            cellprop.m_bbox.set_top(bottom.m_int_value + height.m_uint_value);
        }
        else if (name == "S_CELL_OFFSET")
        {
            oas_assert(property.m_is_standard == true);
            oas_assert(property.m_prop_values.size() == 1);

            const OasisPropValue& offset = property.m_prop_values[0];
            oas_assert(offset.m_type == OasisPropValue::OPVT_UnsignedInteger);
            cellprop.m_offset = offset.m_uint_value;
        }
        else if (name == "PDB_CELL_SIZE")
        {
            oas_assert(property.m_is_standard == false);
            oas_assert(property.m_prop_values.size() == 1);

            const OasisPropValue& cellsize = property.m_prop_values[0];
            oas_assert(cellsize.m_type == OasisPropValue::OPVT_UnsignedInteger);
            cellprop.m_cellsize = cellsize.m_uint_value;
        }
        else if (name == "PDB_IS_TOPCELL")
        {
            oas_assert(property.m_is_standard == false);
            oas_assert(property.m_prop_values.size() == 1);

            const OasisPropValue& istopcell = property.m_prop_values[0];
            oas_assert(istopcell.m_type == OasisPropValue::OPVT_UnsignedInteger);
            oas_assert(istopcell.m_uint_value != 0);
            cellprop.m_is_topcell = (istopcell.m_uint_value != 0);
        }
        else if (name == "PDB_LAYERS")
        {
            oas_assert(property.m_is_standard == false);
            oas_assert(property.m_prop_values.size() >= 1);

            const OasisPropValue &lcount = property.m_prop_values[0];
            oas_assert(lcount.m_type == OasisPropValue::OPVT_UnsignedInteger);
            uint32 count = lcount.m_uint_value;

            int pos = 1;
            for (uint32 i = 0; i < count; ++i)
            {
                const OasisPropValue &layer = property.m_prop_values[pos++];
                oas_assert(layer.m_type == OasisPropValue::OPVT_UnsignedInteger);
                const OasisPropValue &datatype = property.m_prop_values[pos++];
                oas_assert(datatype.m_type == OasisPropValue::OPVT_UnsignedInteger);
                cellprop.m_layers.insert(std::make_pair(layer.m_uint_value, datatype.m_uint_value));
            }
        }
    }
}

bool OasisRepetitionBuilder::partition(const OasisRepetition& repetition, int64 limit, std::vector<OasisRepetition>& results, std::vector<Point>& offsets)
{
    results.clear();
    offsets.clear();
    if(limit<=0 || m_do_partition==0)
    {
        return false;
    }

    Box bbox;
    repetition.get_bbox(bbox);
    if(bbox.width() <= limit && bbox.height() <= limit)
    {
        return false;
    }
    uint64 dimension = repetition.get_dimension();

    switch(repetition.m_type)
    {
        case OasisRepetition::ORT_ReusePrevious:
            return false;
            break;
        case OasisRepetition::ORT_Regular2D:
        {
            if(limit <= repetition.m_spaces[0] && limit <= repetition.m_spaces[1])
            {
                repetition.get_disps(offsets);
                results.resize(offsets.size());
            }
            else if(limit <= repetition.m_spaces[0])
            {
                for(uint32 i = 0; i < repetition.m_dimensionX + 2; ++i)
                {
                    Point offset;
                    offset.set_x(repetition.m_spaces[0] * i);
                    
                    if (repetition.m_spaces[1] < 1)
                    {
                        return false;
                    }
                    uint64 c = (limit + repetition.m_spaces[1] - 1) / repetition.m_spaces[1] + 1;
                    if (c < 2)
                    {
                        return false;
                    }
                    
                    uint64 count = 0;
                    while(count < repetition.m_dimensionY + 2)
                    {
                        offset.set_y(count * repetition.m_spaces[1]);
                        offsets.push_back(offset);
                        results.push_back(OasisRepetition());
                        if(repetition.m_dimensionY + 2 - count > 1)
                        {
                            make_vertical_regular(results.back(), std::min(c, repetition.m_dimensionY + 2 - count), repetition.m_spaces[1]);
                        }
                        count += c;
                    }
                }
            }
            else if(limit <= repetition.m_spaces[1])
            {
                for(uint32 j = 0; j < repetition.m_dimensionY + 2; ++j)
                {
                    Point offset;
                    offset.set_y(repetition.m_spaces[1] * j);

                    if (repetition.m_spaces[0] < 1)
                    {
                        return false;
                    }
                    uint64 c = (limit + repetition.m_spaces[0] - 1) / repetition.m_spaces[0] + 1;
                    if (c < 2)
                    {
                        return false;
                    }

                    uint64 count = 0;
                    while(count < repetition.m_dimensionX + 2)
                    {
                        offset.set_x(count * repetition.m_spaces[0]);
                        offsets.push_back(offset);
                        results.push_back(OasisRepetition());
                        if(repetition.m_dimensionX + 2 - count > 1)
                        {
                            make_horizontal_regular(results.back(), std::min(c, repetition.m_dimensionX + 2 - count), repetition.m_spaces[0]);
                        }
                        count += c;
                    }
                }
            }
            else
            {
                Point offset;
                
                if (repetition.m_spaces[0] < 1)
                {
                    return false;
                }
                uint64 cx = (limit + repetition.m_spaces[0] - 1) / repetition.m_spaces[0] + 1;
                if (cx < 2)
                {
                    return false;
                }

                uint64 countx = 0;
                while(countx < repetition.m_dimensionX + 2)
                {
                    offset.set_x(countx * repetition.m_spaces[0]);

                    if (repetition.m_spaces[1] < 1)
                    {
                        return false;
                    }
                    uint64 cy = (limit + repetition.m_spaces[1] - 1) / repetition.m_spaces[1] + 1;
                    if (cy < 2)
                    {
                        return false;
                    }

                    uint64 county = 0;
                    while(county < repetition.m_dimensionY + 2)
                    {
                        offset.set_y(county * repetition.m_spaces[1]);
                        offsets.push_back(offset);
                        results.push_back(OasisRepetition());
                        if(repetition.m_dimensionY + 2 - county > 1 && repetition.m_dimensionX + 2 - countx > 1)
                        {
                            make_regular_2d(
                                results.back(),
                                std::min(cx, repetition.m_dimensionX + 2 - countx),
                                repetition.m_spaces[0],
                                std::min(cy, repetition.m_dimensionY + 2 - county),
                                repetition.m_spaces[1]
                                );
                        }
                        else if(repetition.m_dimensionY + 2 - county > 1)
                        {
                            make_vertical_regular(results.back(), std::min(cy, repetition.m_dimensionY + 2 - county), repetition.m_spaces[1]);
                        }
                        else if(repetition.m_dimensionX + 2 - countx > 1)
                        {
                            make_horizontal_regular(results.back(), std::min(cx, repetition.m_dimensionX + 2 - countx), repetition.m_spaces[0]);
                        }
                        county += cy;
                    }

                    countx += cx;
                }
            }
            break;
        }
        case OasisRepetition::ORT_HorizontalRegular:
        {
            if(limit <= repetition.m_spaces[0])
            {
                repetition.get_disps(offsets);
                results.resize(offsets.size());
            }
            else
            {
                if (repetition.m_spaces[0] < 1)
                {
                    return false;
                }
                uint64 c = (limit + repetition.m_spaces[0] - 1) / repetition.m_spaces[0] + 1;
                if (c < 2)
                {
                    return false;
                }

                uint64 count = 0;
                while(count < repetition.m_dimensionX + 2)
                {
                    Point offset(count * repetition.m_spaces[0], 0);
                    offsets.push_back(offset);
                    results.push_back(OasisRepetition());
                    if(repetition.m_dimensionX + 2 - count > 1)
                    {
                        make_horizontal_regular(results.back(), std::min(c, repetition.m_dimensionX + 2 - count), repetition.m_spaces[0]);
                    }
                    count += c;
                }
            }
            break;
        }
        case OasisRepetition::ORT_VerticalRegular:
        {
            if(limit <= repetition.m_spaces[0])
            {
                repetition.get_disps(offsets);
                results.resize(offsets.size());
            }
            else
            {
                if (repetition.m_spaces[0] < 1)
                {
                    return false;
                }
                uint64 c = (limit + repetition.m_spaces[0] - 1) / repetition.m_spaces[0] + 1;
                if (c < 2)
                {
                    return false;
                }

                uint64 count = 0;
                while(count < repetition.m_dimensionY + 2)
                {
                    Point offset(0, count * repetition.m_spaces[0]);
                    offsets.push_back(offset);
                    results.push_back(OasisRepetition());
                    if(repetition.m_dimensionY + 2 - count > 1)
                    {
                        make_vertical_regular(results.back(), std::min(c, repetition.m_dimensionY + 2 - count), repetition.m_spaces[0]);
                    }
                    count += c;
                }
            }
            break;
        }
        case OasisRepetition::ORT_HorizontalIrregular:
        case OasisRepetition::ORT_HorizontalIrregularGrid:
        {
            std::vector<Point> disps;
            repetition.get_disps(disps);
            uint32 begin = 0;
            while (begin < disps.size())
            {
                uint32 end = begin + 1;
                while (end < disps.size() && disps[end].x() - disps[begin].x() <= limit)
                {
                    ++end;
                }
                offsets.push_back(disps[begin]);
                results.push_back(OasisRepetition());
                if (end - begin >= 2)
                {
                    make_horizontal_irregular(results.back(), disps, begin, end);
                }
                begin = end;
            }

            break;
        }
        case OasisRepetition::ORT_VerticalIrregular:
        case OasisRepetition::ORT_VerticalIrregularGrid:
        {
            std::vector<Point> disps;
            repetition.get_disps(disps);
            uint32 begin = 0;
            while (begin < disps.size())
            {
                uint32 end = begin + 1;
                while (end < disps.size() && disps[end].y() - disps[begin].y() <= limit)
                {
                    ++end;
                }
                offsets.push_back(disps[begin]);
                results.push_back(OasisRepetition());
                if (end - begin >= 2)
                {
                    make_vertical_irregular(results.back(), disps, begin, end);
                }
                begin = end;
            }

            break;
        }
        case OasisRepetition::ORT_DiagonalRegular2D:
        {
            int64 dn = std::max(abs(repetition.m_deltas[0].dx()), abs(repetition.m_deltas[0].dy()));
            int64 dm = std::max(abs(repetition.m_deltas[1].dx()), abs(repetition.m_deltas[1].dy()));
            if(limit < dn && limit < dm)
            {
                repetition.get_disps(offsets);
                results.resize(offsets.size());
            }
            else if(limit < dn)
            {
                for(uint32 i = 0; i < repetition.m_dimensionN + 2; ++i)
                {
                    Point offsetn(repetition.m_deltas[0].dx() * i, repetition.m_deltas[0].dy() * i);
                    if (dm < 1)
                    {
                        return false;
                    }
                    uint64 c = (limit + dm - 1) / dm + 1;
                    if (c < 2)
                    {
                        return false;
                    }
                    uint64 count = 0;
                    while(count < repetition.m_dimensionM + 2)
                    {
                        Point offsetm(repetition.m_deltas[1].dx() * count, repetition.m_deltas[1].dy() * count);
                        offsets.push_back(offsetn + offsetm);
                        results.push_back(OasisRepetition());
                        if(repetition.m_dimensionM + 2 - count > 1)
                        {
                            make_diagonal_1d(results.back(), std::min(c, repetition.m_dimensionM + 2 - count), repetition.m_deltas[1]);
                        }
                        count += c;
                    }
                }
            }
            else if(limit < dm)
            {
                for(uint32 j = 0; j < repetition.m_dimensionM + 2; ++j)
                {
                    Point offsetm(repetition.m_deltas[1].dx() * j, repetition.m_deltas[1].dy() * j);
                    if (dn < 1)
                    {
                        return false;
                    }
                    uint64 c = (limit + dn - 1) / dn + 1;
                    if (c < 2)
                    {
                        return false;
                    }

                    uint64 count = 0;
                    while(count < repetition.m_dimensionN + 2)
                    {
                        Point offsetn(repetition.m_deltas[0].dx() * count, repetition.m_deltas[0].dy() * count);
                        offsets.push_back(offsetm + offsetn);
                        results.push_back(OasisRepetition());
                        if(repetition.m_dimensionN + 2 - count > 1)
                        {
                            make_diagonal_1d(results.back(), std::min(c, repetition.m_dimensionN + 2 - count), repetition.m_deltas[0]);
                        }
                        count += c;
                    }
                }
            }
            else
            {
                if (dm < 1 || dn < 1)
                {
                    return false;
                }
                uint64 cn = (limit + dn - 1) / dn + 1;
                uint64 cm = (limit + dm - 1) / dm + 1;
                if (cn < 2 || cm < 2)
                {
                    return false;
                }
                uint64 countn = 0;
                while(countn < repetition.m_dimensionN + 2)
                {
                    Point offsetn(repetition.m_deltas[0].dx() * countn, repetition.m_deltas[0].dy() * countn);
                    uint64 countm = 0;
                    while(countm < repetition.m_dimensionM + 2)
                    {
                        Point offsetm(repetition.m_deltas[1].dx() * countm, repetition.m_deltas[1].dy() * countm);
                        offsets.push_back(offsetn + offsetm);
                        results.push_back(OasisRepetition());
                        if(repetition.m_dimensionN + 2 - countn > 1 && repetition.m_dimensionM + 2 - countm > 1)
                        {
                            make_diagonal_2d(
                                results.back(),
                                std::min(cn, repetition.m_dimensionN + 2 - countn),
                                repetition.m_deltas[0],
                                std::min(cm, repetition.m_dimensionM + 2 - countm),
                                repetition.m_deltas[1]
                                );
                        }
                        else if(repetition.m_dimensionN + 2 - countn > 1)
                        {
                            make_diagonal_1d(results.back(), std::min(cn, repetition.m_dimensionN + 2 - countn), repetition.m_deltas[0]);
                        }
                        else if(repetition.m_dimensionM + 2 - countm > 1)
                        {
                            make_diagonal_1d(results.back(), std::min(cm, repetition.m_dimensionM + 2 - countm), repetition.m_deltas[1]);
                        }
                        countm += cm;
                    }
                    countn += cn;
                }
            }
            break;
        }
        case OasisRepetition::ORT_DiagonalRegular1D:
        {
            int64 d = std::max(abs(repetition.m_deltas[0].dx()), abs(repetition.m_deltas[0].dy()));
            if(limit < d)
            {
                repetition.get_disps(offsets);
                results.resize(offsets.size());
            }
            else
            {
                if (d < 1 )
                {
                    return false;
                }
                uint64 c = (limit + d - 1) / d + 1;
                if (c < 2)
                {
                    return false;
                }
                uint64 count = 0;
                while(count < repetition.m_dimension + 2)
                {
                    Point offset(count * repetition.m_deltas[0].dx(), count * repetition.m_deltas[0].dy());
                    offsets.push_back(offset);
                    results.push_back(OasisRepetition());
                    if(repetition.m_dimension + 2 - count > 1)
                    {
                        make_diagonal_1d(results.back(), std::min(c, repetition.m_dimension + 2 - count), repetition.m_deltas[0]);
                    }
                    count += c;
                }
            }
            break;
        }
        case OasisRepetition::ORT_Arbitory:
        case OasisRepetition::ORT_ArbitoryGrid:
        {
            std::vector<Point> disps;
            repetition.get_disps(disps);
            uint32 begin = 0;
            while (begin < disps.size())
            {
                uint32 end = begin + 1;
                while (end < disps.size() && disps[end].x() - disps[begin].x() <= limit && disps[end].y() - disps[begin].y() <= limit)
                {
                    ++end;
                }
                offsets.push_back(disps[begin]);
                results.push_back(OasisRepetition());
                if (end - begin >= 2)
                {
                    make_arbitory(results.back(), disps, begin, end);
                }
                begin = end;
            }

            break;
        }
        default:
            return false;
            break;
    }
    if (results.size() == 1)
    {
        return false;
    }
    OAS_DBG << "repetition partition to " << results.size() << " parts."
        << "box(" << bbox.left() << "," << bbox.bottom() << "," << bbox.right() << "," << bbox.top() << ") "
        << "dimension " << dimension << " limit " << limit  << OAS_ENDL;
    return true;
}

void OasisRepetitionBuilder::make_vertical_regular(OasisRepetition& repetition, uint64 count, int64 space)
{
    oas_assert(count >= 2);
    repetition.m_type = OasisRepetition::ORT_VerticalRegular;
    repetition.m_dimensionY = count - 2;
    repetition.m_spaces.resize(1);
    repetition.m_spaces[0] = space;
}

void OasisRepetitionBuilder::make_horizontal_regular(OasisRepetition& repetition, uint64 count, int64 space)
{
    oas_assert(count >= 2);
    repetition.m_type = OasisRepetition::ORT_HorizontalRegular;
    repetition.m_dimensionX = count - 2;
    repetition.m_spaces.resize(1);
    repetition.m_spaces[0] = space;
}

void OasisRepetitionBuilder::make_regular_2d(OasisRepetition& repetition, uint64 countx, int64 spacex, uint64 county, int64 spacey)
{
    oas_assert(countx >= 2);
    oas_assert(county >= 2);
    repetition.m_type = OasisRepetition::ORT_Regular2D;
    repetition.m_dimensionX = countx - 2;
    repetition.m_dimensionY = county - 2;
    repetition.m_spaces.resize(2);
    repetition.m_spaces[0] = spacex;
    repetition.m_spaces[1] = spacey;
}

void OasisRepetitionBuilder::make_diagonal_1d(OasisRepetition& repetition, uint64 count, const OasisDelta& delta)
{
    oas_assert(count >= 2);
    repetition.m_type = OasisRepetition::ORT_DiagonalRegular1D;
    repetition.m_dimension = count - 2;
    repetition.m_deltas.resize(1);
    repetition.m_deltas[0] = delta;
}

void OasisRepetitionBuilder::make_diagonal_2d(OasisRepetition& repetition, uint64 countn,const OasisDelta& deltan, uint64 countm, const OasisDelta& deltam)
{
    oas_assert(countn >= 2);
    oas_assert(countm >= 2);
    repetition.m_type = OasisRepetition::ORT_DiagonalRegular2D;
    repetition.m_dimensionN = countn - 2;
    repetition.m_dimensionM = countm - 2;
    repetition.m_deltas.resize(2);
    repetition.m_deltas[0] = deltan;
    repetition.m_deltas[1] = deltam;
}

void OasisRepetitionBuilder::make_arbitory(OasisRepetition& repetition, const std::vector<Point>& disps, uint32 begin, uint32 end)
{
    oas_assert(disps.size() >= end);
    oas_assert(end - begin >= 2);
    repetition.m_type = OasisRepetition::ORT_Arbitory;
    repetition.m_dimension = end - begin - 2;
    repetition.m_deltas.resize(end - begin - 1);
    for(uint32 i = 0; i < repetition.m_deltas.size(); ++i)
    {
        repetition.m_deltas[i] = OasisDelta(disps[begin + i + 1] - disps[begin + i]);
    }
}

void OasisRepetitionBuilder::make_vertical_irregular(OasisRepetition& repetition, const std::vector<Point>& disps, uint32 begin, uint32 end)
{
    oas_assert(disps.size() >= end);
    oas_assert(end - begin >= 2);
    repetition.m_type = OasisRepetition::ORT_VerticalIrregular;
    repetition.m_dimensionY = end - begin - 2;
    repetition.m_spaces.resize(end - begin - 1);
    for (uint32 i = 0; i < repetition.m_spaces.size(); ++i)
    {
        repetition.m_spaces[i] = disps[begin + i + 1].y() - disps[begin + i].y();
    }
}

void OasisRepetitionBuilder::make_horizontal_irregular(OasisRepetition& repetition, const std::vector<Point>& disps, uint32 begin, uint32 end)
{
    oas_assert(disps.size() >= end);
    oas_assert(end - begin >= 2);
    repetition.m_type = OasisRepetition::ORT_HorizontalIrregular;
    repetition.m_dimensionX = end - begin - 2;
    repetition.m_spaces.resize(end - begin - 1);
    for (uint32 i = 0; i < repetition.m_spaces.size(); ++i)
    {
        repetition.m_spaces[i] = disps[begin + i + 1].x() - disps[begin + i].x();
    }
}

OasisInterval OasisIntervalBuilder::make_interval(OasisInterval::Type type, uint32 lower, uint32 upper)
{
    OasisInterval val;
    val.m_type = type;
    val.m_lower = lower;
    val.m_upper = upper;
    return val;
}

std::string FormatDetector::detect_format(const std::string& filename)
{
    OasisParser oas_parser;
    oas_parser.open_file(filename);
    if (oas_parser.check_format())
    {
        return "oasis";
    }

    GDSParser gds_parser;
    gds_parser.open_file(filename);
    if (gds_parser.check_format())
    {
        return "gds";
    }

    return "unknown format";
}
