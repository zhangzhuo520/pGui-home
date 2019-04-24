#ifndef OASIS_MODALVARIABLE_H
#define OASIS_MODALVARIABLE_H
#include "oasis_types.h"
#include "oasis_cell.h"

namespace oasis
{
class ModalVariable
{
public:
    enum XYModeType { XYAbsolute, XYRelative };
    enum DefineBit
    {
        RepetitionBit = 0,
        PlacementXBit,
        PlacementYBit,
        PlacementCellBit,
        LayerBit,
        DataTypeBit,
        TextLayerBit,
        TextTypeBit,
        TextXBit,
        TextYBit,
        TextStringBit,
        GeometryXBit,
        GeometryYBit,
        XYModeBit,
        GeometryWBit,
        GeometryHBit,
        PolygonPointListBit,
        PathHalfWidthBit,
        PathPointListBit,
        PathStartExtBit,
        PathEndExtBit,
        CTrapezoidTypeBit,
        CircleRadiusBit,
        LastPropnameBit,
        LastPropvaluesBit,
        LastPropStandardBit,
        LastPropertyBit,

        DefineMax
    };

    ModalVariable()
    {
        m_xymode = XYAbsolute;
        m_last_property_is_standard = false;
    }
    ~ModalVariable() {}
    void reset();
    void define_bit(DefineBit bit) { m_defined |= (1 << bit); }
    bool defined_bit(DefineBit bit) const { return (m_defined & (1 << bit)) != 0;}

    OasisRepetition m_repetition;
    int64 m_placement_x;
    int64 m_placement_y;
    int64 m_placement_cell;
    uint64 m_layer;
    uint64 m_datatype;
    uint64 m_textlayer;
    uint64 m_texttype;
    int64 m_text_x;
    int64 m_text_y;
    OasisTextString m_textstring;
    int64 m_geometry_x;
    int64 m_geometry_y;
    XYModeType m_xymode;
    uint64 m_geometry_w;
    uint64 m_geometry_h;
    OasisPointList m_polygon_pointlist;
    uint64 m_path_halfwidth;
    OasisPointList m_path_pointlist;
    int64 m_path_start_extention;
    int64 m_path_end_extention;
    uint64 m_ctrapezoid_type;
    uint64 m_circle_radius;
    OasisPropName m_last_propname;
    std::vector<OasisPropValue> m_last_propvalues;
    bool m_last_property_is_standard;

private:
    uint64 m_defined;
};
}
#endif
