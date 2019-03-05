#include "oasis_cell.h"
#include "oasis_layout.h"
#include "oasis_util.h"
#include "oasis_parser.h"
#include "cell_visitor.h"

#include <sstream>
#include <algorithm>

#include <cmath>
#include <cfloat>

using namespace std;
using namespace oasis;

void OasisProperty::create(bool is_std, const OasisPropName& propname, const std::vector<OasisPropValue>& propvalues)
{
    m_is_standard = is_std;
    m_prop_name = propname;
    m_prop_values = propvalues;
}

void OasisProperty::write(OasisWriter& writer)
{
    using namespace PropertyInfoByte;

    ModalVariable& mv = writer.get_modal_variables();
    uint8 info_byte = 0;

    bool same_standard = mv.defined_bit(ModalVariable::LastPropStandardBit) && mv.m_last_property_is_standard == m_is_standard;
    bool same_name = mv.defined_bit(ModalVariable::LastPropnameBit) && mv.m_last_propname == m_prop_name;
    bool same_value = mv.defined_bit(ModalVariable::LastPropvaluesBit) && mv.m_last_propvalues == m_prop_values;

    if(same_standard && same_name && same_value)
    {
        writer.write_record_id(ORID_PropertyLast);
        return;
    }

    if(m_is_standard)
    {
        info_byte |= IsStandard;
    }

    if(same_value)
    {
        info_byte |= ReuseLastValue;
    }
    else
    {
        if(m_prop_values.size() >= 15)
        {
            info_byte |= ValueCountMask;
        }
        else
        {
            info_byte |= ((uint8)m_prop_values.size() << 4);
        }
    }

    if(!same_name)
    {
        info_byte |= HasExplicitName;

        if(m_prop_name.is_referenced())
        {
            info_byte |= HasRefnum;
        }
    }

    writer.write_record_id(ORID_Property);
    writer.write_byte(info_byte);

    if(!same_name)
    {
        if(m_prop_name.is_referenced())
        {
            writer.write_uint(m_prop_name.get_index());//check
        }
        else
        {
            mv.m_last_propname = m_prop_name;
            mv.define_bit(ModalVariable::LastPropnameBit);
            writer.write_string(m_prop_name.get_name());
        }
    }

    if(!same_value)
    {
        if(m_prop_values.size() >= 15)
        {
            writer.write_uint(m_prop_values.size());
        }

        for(size_t i = 0; i < m_prop_values.size(); ++i)
        {
            writer.write_prop_value(m_prop_values[i]);
        }

        mv.m_last_propvalues = m_prop_values;
        mv.define_bit(ModalVariable::LastPropvaluesBit);
    }

    mv.m_last_property_is_standard = m_is_standard;
    mv.define_bit(ModalVariable::LastPropStandardBit);
}

void OasisElement::write_repetition(OasisWriter& writer)
{
    oas_assert(has_repetition());

    ModalVariable& mv = writer.get_modal_variables();

    if(mv.defined_bit(ModalVariable::RepetitionBit) && mv.m_repetition == *m_repetition)
    {
        writer.write_byte(OasisRepetition::ORT_ReusePrevious);
    }
    else
    {
        mv.m_repetition = *m_repetition;
        mv.define_bit(ModalVariable::RepetitionBit);
        writer.write_repetition(mv.m_repetition);
    }
}

void OasisElement::set_repetition(const OasisRepetition& repetition)
{
    if (m_repetition)
    {
        delete m_repetition;
        m_repetition = NULL;
    }
    m_repetition = new OasisRepetition(repetition);
}

void OasisElement::get_disps(std::vector<Point>& disps) const
{
    disps.clear();
    if(!m_repetition)
    {
        disps.push_back(Point());
        return;
    }

    m_repetition->get_disps(disps);
}

void OasisElement::get_bbox_with_repetition(Box &box)
{
    if (has_repetition())
    {
        Box box0;
        bbox(box0);
        Box rbox;
        m_repetition->get_bbox(rbox);
        box.set_left(rbox.left() + box0.left());
        box.set_bottom(rbox.bottom() + box0.bottom());
        box.set_right(rbox.right() + box0.right());
        box.set_top(rbox.top() + box0.top());
    }
    else
    {
        bbox(box);
    }
}

void RectangleElement::create(int64 x, int64 y, uint64 w, uint64 h, uint64 layer, uint64 datatype, OasisRepetition* repetition)
{
    m_rect.set_left(x);
    m_rect.set_bottom(y);
    m_rect.set_right(x + w);
    m_rect.set_top(y + h);

    set_ld(layer, datatype);
    if(repetition)
    {
        set_repetition(*repetition);
    }
}

void RectangleElement::create(const Box &box, uint64 layer, uint64 datatype, OasisRepetition *repetition)
{
    create(box.left(), box.bottom(), box.right() - box.left(), box.top() - box.bottom(), layer, datatype, repetition);
}

OasisElement* RectangleElement::clone()
{
    RectangleElement* e = new RectangleElement;
    e->set_base(this);
    e->m_rect = m_rect;
    return e;
}

void RectangleElement::polygon(Polygon& p)
{
    std::vector<Point> pts;
    pts.reserve(4);
    pts.push_back(Point(m_rect.left(), m_rect.bottom()));
    pts.push_back(Point(m_rect.left(), m_rect.top()));
    pts.push_back(Point(m_rect.right(), m_rect.top()));
    pts.push_back(Point(m_rect.right(), m_rect.bottom()));
    p.assign(pts.begin(), pts.end());
}

void RectangleElement::write(OasisWriter& writer)
{
    using namespace RectangleInfoByte;

    ModalVariable& mv = writer.get_modal_variables();
    uint8 info_byte = 0;

    if(!mv.defined_bit(ModalVariable::LayerBit) || mv.m_layer != m_layer)
    {
        info_byte |= HasLayer;
    }

    if(!mv.defined_bit(ModalVariable::DataTypeBit) || mv.m_datatype != m_datatype)
    {
        info_byte |= HasDatatype;
    }

    if(m_rect.width() == m_rect.height())
    {
        info_byte |= IsSquare;
        if(!mv.defined_bit(ModalVariable::GeometryHBit) || mv.m_geometry_h != (uint64)m_rect.height())
        {
            mv.m_geometry_h = m_rect.height();
            mv.define_bit(ModalVariable::GeometryHBit);
        }
    }
    else
    {
        if(!mv.defined_bit(ModalVariable::GeometryHBit) || mv.m_geometry_h != (uint64)m_rect.height())
        {
            info_byte |= HasHeight;
        }
    }

    if(!mv.defined_bit(ModalVariable::GeometryWBit) || mv.m_geometry_w != (uint64)m_rect.width())
    {
        info_byte |= HasWidth;
    }

    if(mv.m_geometry_x != m_rect.left())
    {
        info_byte |= HasX;
    }

    if(mv.m_geometry_y != m_rect.bottom())
    {
        info_byte |= HasY;
    }

    if(has_repetition())
    {
        info_byte |= HasRepetition;
    }

    writer.write_record_id(ORID_Rectangle);
    writer.write_byte(info_byte);

    if(info_byte & HasLayer)
    {
        mv.m_layer = m_layer;
        mv.define_bit(ModalVariable::LayerBit);
        writer.write_uint(mv.m_layer);
    }

    if(info_byte & HasDatatype)
    {
        mv.m_datatype = m_datatype;
        mv.define_bit(ModalVariable::DataTypeBit);
        writer.write_uint(mv.m_datatype);
    }

    if(info_byte & HasWidth)
    {
        mv.m_geometry_w = m_rect.width();
        mv.define_bit(ModalVariable::GeometryWBit);
        writer.write_uint(mv.m_geometry_w);
    }

    if(info_byte & HasHeight)
    {
        mv.m_geometry_h = m_rect.height();
        mv.define_bit(ModalVariable::GeometryHBit);
        writer.write_uint(mv.m_geometry_h);
    }

    if(info_byte & HasX)
    {
        mv.m_geometry_x = m_rect.left();
        mv.define_bit(ModalVariable::GeometryXBit);
        writer.write_int(mv.m_geometry_x);
    }

    if(info_byte & HasY)
    {
        mv.m_geometry_y = m_rect.bottom();
        mv.define_bit(ModalVariable::GeometryYBit);
        writer.write_int(mv.m_geometry_y);
    }

    if(info_byte & HasRepetition)
    {
        write_repetition(writer);
    }

    //write element properties
}

bool RectangleElement::compare(OasisElement* rh)
{
    RectangleElement* b = dynamic_cast<RectangleElement*>(rh);
    oas_assert(b != NULL);
    if(m_rect.width() != b->m_rect.width())
    {
        return m_rect.width() < b->m_rect.width();
    }
    if(m_rect.height() != b->m_rect.height())
    {
        return m_rect.height() < b->m_rect.height();
    }
    if(m_rect.left() != b->m_rect.left())
    {
        return m_rect.left() < b->m_rect.left();
    }
    if(m_rect.bottom() != b->m_rect.bottom())
    {
        return m_rect.bottom() < b->m_rect.bottom();
    }
    return this < rh;
}

bool RectangleElement::equal(OasisElement* rh)
{
    RectangleElement* b = dynamic_cast<RectangleElement*>(rh);
    oas_assert(b != NULL);
    return m_rect.width() == b->m_rect.width() && m_rect.height() == b->m_rect.height();
}

void PolygonElement::create(const std::vector<Point>& points, uint64 layer, uint64 datatype, OasisRepetition* repetition)
{
    if (points.size() < 3)
    {
        if (points.size() > 0)
        {
            OAS_INFO << "WARNING: at (" << points[0].x() << ", " << points[0].y() << "): " << "PolygonElement::create(): skipped. invalid vertices size = " << points.size();
        }
        else
        {
            OAS_INFO << "PolygonElement::create(): skipped. invalid vertices size = 0";
        }
        m_valid = 0;
        return;
    }
   
    const Point& p0 = points[0];
    std::vector<Point> newpoints;
    newpoints.reserve(points.size());
    Point prev;
    for(uint32 i = 0; i < points.size(); ++i)
    {
        if (i != 0 && points[i] == points[i - 1])
        {
            continue;
        }
        newpoints.push_back(points[i] - p0);
    }
    create(p0.x(), p0.y(), newpoints, layer, datatype, repetition);
}

void PolygonElement::create(int64 x, int64 y, const std::vector<Point>& points, uint64 layer, uint64 datatype, OasisRepetition* repetition)
{
    if (points.size() < 2)
    {
        OAS_INFO << "WARNING: at (" << x << ", " << y << "): " << "PolygonElement::create(): skipped. invalid vertices size = " << points.size();
        m_valid = 0;
        return;
    }
   
    m_start_point.set_x(x);
    m_start_point.set_y(y);
    OasisPointListBuilder builder(true);
    if (!builder.points_to_list(points, m_point_list))
    {
        OAS_INFO << "WARNING: at (" << x << ", " << y << "): " << "PolygonElement::create(): skipped. point values invalid";
        m_valid = 0;
        return;
    }

    set_ld(layer, datatype);
    if(repetition)
    {
        set_repetition(*repetition);
    }
}

void PolygonElement::create(int64 x, int64 y, const OasisPointList& pointlist, uint64 layer, uint64 datatype, OasisRepetition* repetition)
{
    if (pointlist.size() < 2)
    {
        OAS_INFO << "WARNING: at (" << x << ", " << y << "): " << "PolygonElement::create(): skipped. invalid vertices size = " << pointlist.size();
        m_valid = 0;
        return;
    }
   
    m_start_point.set_x(x);
    m_start_point.set_y(y);
    m_point_list = pointlist;

    set_ld(layer, datatype);
    if(repetition)
    {
        set_repetition(*repetition);
    }
}

OasisElement* PolygonElement::clone()
{
    PolygonElement* e = new PolygonElement;
    e->set_base(this);
    e->m_start_point = m_start_point;
    e->m_point_list = m_point_list;
    return e;
}

void PolygonElement::polygon(Polygon& p)
{
    std::vector<Point> points;
    OasisPointListBuilder builder(true);
    if (!builder.list_to_points(m_point_list, points))
    {
        OAS_INFO << "WARNING: invalid polygon element data at (" << m_start_point.x() << ", " << m_start_point.y() << "). skipped returning polygon" << OAS_ENDL;
        return;
    }
    
    for(uint32 i = 0; i < points.size(); ++i)
    {
        points[i] = points[i] + m_start_point;
    }
    p.assign(points.begin(), points.end());
}

void PolygonElement::bbox(Box& box)
{
    std::vector<Point> points;
    OasisPointListBuilder builder(true);
    if (!builder.list_to_points(m_point_list, points))
    {
        OAS_INFO << "WARNING: invalid polygon element data at (" << m_start_point.x() << ", " << m_start_point.y() << "). skipped calculate bbox" << OAS_ENDL;
        return;
    }

    for(uint32 i = 0; i < points.size(); ++i)
    {
        box.update(points[i] + m_start_point);
    }
}

void PolygonElement::write(OasisWriter& writer)
{
    using namespace PolygonInfoByte;

    ModalVariable& mv = writer.get_modal_variables();
    uint8 info_byte = 0;

    if(!mv.defined_bit(ModalVariable::LayerBit) || mv.m_layer != m_layer)
    {
        info_byte |= HasLayer;
    }

    if(!mv.defined_bit(ModalVariable::DataTypeBit) || mv.m_datatype != m_datatype)
    {
        info_byte |= HasDatatype;
    }

    if(mv.m_geometry_x != m_start_point.x())
    {
        info_byte |= HasX;
    }

    if(mv.m_geometry_y != m_start_point.y())
    {
        info_byte |= HasY;
    }

    if(!mv.defined_bit(ModalVariable::PolygonPointListBit) || mv.m_polygon_pointlist != m_point_list)
    {
        info_byte |= HasPointList;
    }

    if(has_repetition())
    {
        info_byte |= HasRepetition;
    }

    writer.write_record_id(ORID_Polygon);
    writer.write_byte(info_byte);

    if(info_byte & HasLayer)
    {
        mv.m_layer = m_layer;
        mv.define_bit(ModalVariable::LayerBit);
        writer.write_uint(mv.m_layer);
    }

    if(info_byte & HasDatatype)
    {
        mv.m_datatype = m_datatype;
        mv.define_bit(ModalVariable::DataTypeBit);
        writer.write_uint(mv.m_datatype);
    }

    if(info_byte & HasPointList)
    {
        mv.m_polygon_pointlist = m_point_list;
        mv.define_bit(ModalVariable::PolygonPointListBit);
        writer.write_point_list(mv.m_polygon_pointlist);
    }

    if(info_byte & HasX)
    {
        mv.m_geometry_x = m_start_point.x();
        mv.define_bit(ModalVariable::GeometryXBit);
        writer.write_int(mv.m_geometry_x);
    }

    if(info_byte & HasY)
    {
        mv.m_geometry_y = m_start_point.y();
        mv.define_bit(ModalVariable::GeometryYBit);
        writer.write_int(mv.m_geometry_y);
    }

    if(info_byte & HasRepetition)
    {
        write_repetition(writer);
    }

    //TODO write element properties, do not support
}

bool PolygonElement::compare(OasisElement* rh)
{
    PolygonElement* b = dynamic_cast<PolygonElement*>(rh);
    oas_assert(b != NULL);
    if(m_point_list != b->m_point_list)
    {
        return m_point_list < b->m_point_list;
    }
    if(m_start_point != b->m_start_point)
    {
        return m_start_point < b->m_start_point;
    }
    return this < rh;
}

bool PolygonElement::equal(OasisElement* rh)
{
    PolygonElement* b = dynamic_cast<PolygonElement*>(rh);
    oas_assert(b != NULL);
    return m_point_list == b->m_point_list;
}

void PathElement::create(int64 x, int64 y, int64 startext, int64 endext, uint64 halfwidth, const OasisPointList& pointlist, uint64 layer, uint64 datatype, OasisRepetition* repetition)
{
    m_start_point.set_x(x);
    m_start_point.set_y(y);
    m_start_ext = startext;
    m_end_ext = endext;
    m_halfwidth = halfwidth;
    m_point_list = pointlist;

    set_ld(layer, datatype);
    if(repetition)
    {
        set_repetition(*repetition);
    }
}

void PathElement::create(int64 startext, int64 endext, uint64 halfwidth, const std::vector<Point>& points, uint64 layer, uint64 datatype, OasisRepetition* repetition)
{
    const Point& p0 = points[0];
    std::vector<Point> newpoints;
    newpoints.reserve(points.size());
    for(uint32 i = 0; i < points.size(); ++i)
    {
        newpoints.push_back(points[i] - p0);
    }
    create(p0.x(), p0.y(), startext, endext, halfwidth, newpoints, layer, datatype, repetition);
}

void PathElement::create(int64 x, int64 y, int64 startext, int64 endext, uint64 halfwidth, const std::vector<Point>& points, uint64 layer, uint64 datatype, OasisRepetition* repetition)
{
    m_start_point.set_x(x);
    m_start_point.set_y(y);
    m_start_ext = startext;
    m_end_ext = endext;
    m_halfwidth = halfwidth;
    OasisPointListBuilder builder(false);
    if (!builder.points_to_list(points, m_point_list))
    {
        OAS_INFO << "WARNING: at (" << x << ", " << y << "): " << "PathElement::create(): skipped. points value invalid";
        m_valid = 0;
        return;
    }

    set_ld(layer, datatype);
    if(repetition)
    {
        set_repetition(*repetition);
    }
}

OasisElement* PathElement::clone()
{
    PathElement* e = new PathElement;
    e->set_base(this);
    e->m_start_point = m_start_point;
    e->m_start_ext = m_start_ext;
    e->m_end_ext = m_end_ext;
    e->m_halfwidth = m_halfwidth;
    e->m_point_list = m_point_list;
    return e;
}


static void intersectPath( const Point& A, // A->B = current centerline path.
                           const Point& B, // B->C = next centerline path.
                           const Point& C,
                           uint32 halfwidth,
                           Point& poly_pre, // previous vertex on expanded polygon for 45-degree correction.
                           Point& poly_new, // the new vertex on the polygon
                           bool on_right)        // the right or left side of the path.
{
    // L1 (L2) = line parallel to A->B (B->C) at distance halfwidth on appropriate side
    float64 L1dx = B.x() - A.x(), L1dy = B.y() - A.y(), L2dx = C.x() - B.x(), L2dy = C.y() - B.y();
    float64 L1r = std::sqrt(L1dx * L1dx + L1dy * L1dy), L2r = std::sqrt(L2dx * L2dx + L2dy * L2dy);
    float64 L1dr = halfwidth / L1r, L2dr = halfwidth / L2r;
    float64 XS = on_right ? 1 : -1, YS = on_right ? -1 : 1;
    float64 L1x1 = A.x() + XS * L1dr * L1dy, L1y1 = A.y() + YS * L1dr * L1dx;
    float64 L2x1 = B.x() + XS * L2dr * L2dy, L2y1 = B.y() + YS * L2dr * L2dx;
    float64 L = (( L2y1 - L1y1 ) * L1dx - ( L2x1 - L1x1 ) * L1dy) / (L2dx * L1dy - L2dy * L1dx);

    poly_new.set_x((int64)rint( L2x1 + L * L2dx ));  // save the intersection point
    poly_new.set_y((int64)rint( L2y1 + L * L2dy ));

    //if (( B.x() == C.x() ) || ( B.y() == C.y() )) return;

    bool is_45 = ((( B.x() - A.x() ) == ( B.y() - A.y() )) || (( B.x() - A.x() ) == ( A.y() - B.y() ))) ? true : false;
    
    if( is_45 )
    { // The centerline is 45-degree.
        bool now_45 = ( (( poly_new.x() - poly_pre.x() ) == ( poly_new.y() - poly_pre.y() )) ||
                        (( poly_new.x() - poly_pre.x() ) == ( poly_pre.y() - poly_new.y() )) ) ? true : false;
        
        if( !now_45 )
        { // if polygon is not 45, calc the closest halfwidth to B->C:
            int64 try_x = 0, try_y = 0, best_try_x = LONG_MAX, best_try_y = LONG_MAX;
            float64 best_d = DBL_MAX;
            for( uint32 i = 0; i < 8; i++ )
            {
                switch( i ) {
                    case 0:
                        try_x = poly_new.x() + 0;
                        try_y = poly_new.y() + 1;
                        break;
                    case 1:
                        try_x = poly_new.x() + 0;
                        try_y = poly_new.y() - 1;
                        break;
                    case 2:
                        try_x = poly_new.x() + 1;
                        try_y = poly_new.y() + 0;
                        break;
                    case 3:
                        try_x = poly_new.x() - 1;
                        try_y = poly_new.y() + 0;
                        break;
                    case 4:
                        try_x = poly_new.x() + 1;
                        try_y = poly_new.y() + 1;
                        break;
                    case 5:
                        try_x = poly_new.x() + 1;
                        try_y = poly_new.y() - 1;
                        break;
                    case 6:
                        try_x = poly_new.x() - 1;
                        try_y = poly_new.y() + 1;
                        break;
                    case 7:
                        try_x = poly_new.x() - 1;
                        try_y = poly_new.y() - 1;
                        break;
                }

                now_45 = ( (( try_x - poly_pre.x() ) == ( try_y - poly_pre.y() )) ||
                           (( try_x - poly_pre.x() ) == ( poly_pre.y() - try_y )) ) ? true : false;
                if (now_45)
                { 
                    float64 d = fabs( (( try_y - L2y1 ) * L2dx ) - (( try_x - L2x1 ) * L2dy ));
                    if (fabs( d - halfwidth ) < best_d)
                    {
                        best_try_x = try_x;
                        best_try_y = try_y;
                        best_d = fabs(d - halfwidth);
                    }
                }
            }
            if (best_try_x != LONG_MAX)
            {
                poly_new.set_x(best_try_x);
                poly_new.set_y(best_try_y);
                return;
            }
        }
    }
}

static void expandPathToPolygon(std::vector<Point>& path,
                                std::vector<Point>& poly,
                                int halfwidth, int startext, int endext)
{
    uint32 r = 0; // r moves along the right
    uint32 l = 2 * path.size() - 1; // l moves along the left

    poly.resize(path.size()*2);
    
    if( path[0].x() == path[1].x() )
    { // First edge vertical.
        if( path[0].y() < path[1].y() )
        {
            poly[r].set_x(path[0].x() + halfwidth);
            poly[l].set_x(path[0].x() - halfwidth);
            poly[r].set_y(path[0].y() - startext);
            poly[l].set_y(path[0].y() - startext);
        }
        else
        {
            poly[r].set_x(path[0].x() - halfwidth);
            poly[l].set_x(path[0].x() + halfwidth);
            poly[r].set_y(path[0].y() + startext);
            poly[l].set_y(path[0].y() + startext);
        }
    }
    else if( path[0].y() == path[1].y() )
    { // First edge horizontal.
        if( path[0].x() < path[1].x() )
        {
            poly[r].set_y(path[0].y() - halfwidth);
            poly[l].set_y(path[0].y() + halfwidth);
            poly[r].set_x(path[0].x() - startext);
            poly[l].set_x(path[0].x() - startext);
        }
        else
        {
            poly[r].set_y(path[0].y() + halfwidth);
            poly[l].set_y(path[0].y() - halfwidth);
            poly[r].set_x(path[0].x() + startext);
            poly[l].set_x(path[0].x() + startext);
        }                                                   
    }
    else
    { // First edge diagonal.
        float64 DX = path[1].x() - path[0].x();
        float64 DY = path[1].y() - path[0].y();
        float64 R = std::sqrt(DX * DX + DY * DY);
        int32 DDX = (int32)rint((startext * DX - halfwidth * DY) / R);
        int32 DDY = (int32)rint((startext * DY + halfwidth * DX) / R);
        poly[r].set_x(path[0].x() - DDX);
        poly[r].set_y(path[0].y() - DDY); 
        poly[l].set_x(path[0].x() - DDY);
        poly[l].set_y(path[0].y() + DDX);
    }
    
    r++;
    l--;
    uint32 i = 1;
    
    for (; i < path.size()-1; ++i, ++r, --l)
    {
        if (path[i].x() == path[i-1].x())
        {     // First edge is vertical
            if (path[i].y() == path[i+1].y())
            {    // Second edge is horizontal 
                if (path[i].y() > path[i-1].y())
                {  // Case 5 & Case 1
                    poly[r].set_x(path[i].x() + halfwidth);
                    poly[l].set_x(path[i].x() - halfwidth);
                    if (path[i].x() < path[i+1].x())
                    {  // Case 1
                        poly[r].set_y(path[i].y() - halfwidth);
                        poly[l].set_y(path[i].y() + halfwidth);
                    }
                    else
                    {  // Case 5
                        poly[r].set_y(path[i].y() + halfwidth);
                        poly[l].set_y(path[i].y() - halfwidth);
                    }
                }
                else
                {  // Case 4 & Case 8
                    poly[r].set_x(path[i].x() - halfwidth);
                    poly[l].set_x(path[i].x() + halfwidth);
                    if (path[i].x() < path[i+1].x())
                    {  // Case 8
                        poly[r].set_y(path[i].y() - halfwidth);
                        poly[l].set_y(path[i].y() + halfwidth);
                    }
                    else
                    {  // Case 4
                        poly[r].set_y(path[i].y() + halfwidth);
                        poly[l].set_y(path[i].y() - halfwidth);
                    }
                }
            }
            else
            {  // Second edge is diagonal
                intersectPath( path[i - 1], path[i], path[i + 1], halfwidth, poly[r - 1], poly[r], true );
                intersectPath( path[i - 1], path[i], path[i + 1], halfwidth, poly[l + 1], poly[l], false );
            }
        }
        else if (path[i].y() == path[i-1].y())
        {    // First edge is horizontal
            if (path[i].x() == path[i+1].x())
            {    // Second edge is vertical
                if (path[i].x() > path[i-1].x())
                {  // Case 2 & Case 6
                    if (path[i].y() < path[i+1].y())
                    {  // Case 2
                        poly[r].set_x(path[i].x() + halfwidth);
                        poly[l].set_x(path[i].x() - halfwidth);
                    }
                    else
                    {  // Case 6
                        poly[r].set_x(path[i].x() - halfwidth);
                        poly[l].set_x(path[i].x() + halfwidth);
                    } 
                    poly[r].set_y(path[i].y() - halfwidth);
                    poly[l].set_y(path[i].y() + halfwidth);
                }
                else
                {  // Case 3 & Case 7
                    if (path[i].y() < path[i+1].y())
                    {  // Case 7
                        poly[r].set_x(path[i].x() + halfwidth);
                        poly[l].set_x(path[i].x() - halfwidth);
                    }
                    else
                    {  // Case 3
                        poly[r].set_x(path[i].x() - halfwidth);
                        poly[l].set_x(path[i].x() + halfwidth);
                    }   
                    poly[r].set_y(path[i].y() + halfwidth);
                    poly[l].set_y(path[i].y() - halfwidth);
                }
            }
            else
            {  // Second edge is diagonal
                intersectPath( path[i - 1], path[i], path[i + 1], halfwidth, poly[r - 1], poly[r], true );
                intersectPath( path[i - 1], path[i], path[i + 1], halfwidth, poly[l + 1], poly[l], false );
            }
        }
        else
        {  // First edge is diagonal
            intersectPath( path[i - 1], path[i], path[i + 1], halfwidth, poly[r - 1], poly[r], true );
            intersectPath( path[i - 1], path[i], path[i + 1], halfwidth, poly[l + 1], poly[l], false );
        }
    }
    
    if( path[i].x() == path[i-1].x() )
    {  // Last edge is vertical
        if( path[i].y() < path[i-1].y() )
        {
            poly[r].set_x(path[i].x() - halfwidth);
            poly[l].set_x(path[i].x() + halfwidth);
            poly[r].set_y(path[i].y() - endext);
            poly[l].set_y(path[i].y() - endext);
        }
        else
        {
            poly[r].set_x(path[i].x() + halfwidth);
            poly[l].set_x(path[i].x() - halfwidth);
            poly[r].set_y(path[i].y() + endext);
            poly[l].set_y(path[i].y() + endext);
        }
    }
    else if( path[i].y() == path[i-1].y() )
    {    // Last edge is horizontal
        if( path[i].x() < path[i-1].x() )
        {
            poly[r].set_y(path[i].y() + halfwidth);
            poly[l].set_y(path[i].y() - halfwidth);
            poly[r].set_x(path[i].x() - endext);
            poly[l].set_x(path[i].x() - endext);
        }
        else
        {
            poly[r].set_y(path[i].y() - halfwidth);
            poly[l].set_y(path[i].y() + halfwidth);
            poly[r].set_x(path[i].x() + endext);
            poly[l].set_x(path[i].x() + endext);
        }                                                   
    }
    else
    {  // Last edge is diagonal
        float64 DX = path[i].x() - path[i-1].x();
        float64 DY = path[i].y() - path[i-1].y();
        float64 R = sqrt(DX * DX + DY * DY);
        int32 DDX = (int32)rint((endext * DX + halfwidth * DY) / R);
        int32 DDY = (int32)rint((endext * DY - halfwidth * DX) / R);
        poly[r].set_x(path[i].x() + DDX);
        poly[r].set_y(path[i].y() + DDY); 
        poly[l].set_x(path[i].x() - DDY);
        poly[l].set_y(path[i].y() + DDX);
    }
}

void PathElement::polygon(Polygon& p)
{
    std::vector<Point> path_points;
    OasisPointListBuilder builder(false);
    if(!builder.list_to_points(m_point_list, path_points))
    {
        OAS_INFO << "WARNING: invalid path element data at (" << m_start_point.x() << ", " << m_start_point.y() << "). skipped returning polygon" << OAS_ENDL;
        return;
    }
    for(uint32 i = 0; i < path_points.size(); ++i)
    {
        path_points[i] = path_points[i] + m_start_point;
    }
    
    std::vector<Point> polygon_points;
    expandPathToPolygon(path_points, polygon_points, m_halfwidth, m_start_ext, m_end_ext);
    p.assign(polygon_points.begin(), polygon_points.end());
}

void PathElement::bbox(Box& box)
{
    Polygon p;
    polygon(p);
    box = p.bbox();
}

void PathElement::write(OasisWriter& writer)
{
    using namespace PathInfoByte;

    ModalVariable& mv = writer.get_modal_variables();
    uint8 info_byte = 0;

    if(!mv.defined_bit(ModalVariable::LayerBit) || mv.m_layer != m_layer)
    {
        info_byte |= HasLayer;
    }

    if(!mv.defined_bit(ModalVariable::DataTypeBit) || mv.m_datatype != m_datatype)
    {
        info_byte |= HasDatatype;
    }

    if(mv.m_geometry_x != m_start_point.x())
    {
        info_byte |= HasX;
    }

    if(mv.m_geometry_y != m_start_point.y())
    {
        info_byte |= HasY;
    }

    if(!mv.defined_bit(ModalVariable::PathPointListBit) || mv.m_path_pointlist != m_point_list)
    {
        info_byte |= HasPointList;
    }

    if(!mv.defined_bit(ModalVariable::PathStartExtBit) || !mv.defined_bit(ModalVariable::PathEndExtBit) || mv.m_path_start_extention != m_start_ext || mv.m_path_end_extention != m_end_ext)
    {
        info_byte |= HasExtension;
    }

    if(!mv.defined_bit(ModalVariable::PathHalfWidthBit) || mv.m_path_halfwidth != m_halfwidth)
    {
        info_byte |= HasHalfWidth;
    }

    if(has_repetition())
    {
        info_byte |= HasRepetition;
    }

    writer.write_record_id(ORID_Path);
    writer.write_byte(info_byte);

    if(info_byte & HasLayer)
    {
        mv.m_layer = m_layer;
        mv.define_bit(ModalVariable::LayerBit);
        writer.write_uint(mv.m_layer);
    }

    if(info_byte & HasDatatype)
    {
        mv.m_datatype = m_datatype;
        mv.define_bit(ModalVariable::DataTypeBit);
        writer.write_uint(mv.m_datatype);
    }

    if(info_byte & HasHalfWidth)
    {
        mv.m_path_halfwidth = m_halfwidth;
        mv.define_bit(ModalVariable::PathHalfWidthBit);
        writer.write_uint(mv.m_path_halfwidth);
    }


    if(info_byte & HasExtension)
    {
        uint8 ext_scheme = 0;
        if(mv.defined_bit(ModalVariable::PathStartExtBit) && mv.m_path_start_extention == m_start_ext)
        {
        }
        else if(m_start_ext == 0)
        {
            ext_scheme |= FlushExtension;
        }
        else if(m_start_ext == (int64)m_halfwidth)
        {
            ext_scheme |= HalfwidthExtension;
        }
        else
        {
            ext_scheme |= ExplicitExtension;
        }
        ext_scheme <<= 2;

        if(mv.defined_bit(ModalVariable::PathEndExtBit) && mv.m_path_end_extention == m_end_ext)
        {
        }
        else if(m_end_ext == 0)
        {
            ext_scheme |= FlushExtension;
        }
        else if(m_end_ext == (int64)m_halfwidth)
        {
            ext_scheme |= HalfwidthExtension;
        }
        else
        {
            ext_scheme |= ExplicitExtension;
        }

        writer.write_byte(ext_scheme);

        if(((ext_scheme >> 2) & ExplicitExtension) == ExplicitExtension)
        {
            writer.write_int(m_start_ext);
        }
        if((ext_scheme & ExplicitExtension) == ExplicitExtension)
        {
            writer.write_int(m_end_ext);
        }

        mv.m_path_start_extention = m_start_ext;
        mv.m_path_end_extention = m_end_ext;
        mv.define_bit(ModalVariable::PathStartExtBit);
        mv.define_bit(ModalVariable::PathEndExtBit);
    }

    if(info_byte & HasPointList)
    {
        mv.m_path_pointlist = m_point_list;
        mv.define_bit(ModalVariable::PathPointListBit);
        writer.write_point_list(mv.m_path_pointlist);
    }

    if(info_byte & HasX)
    {
        mv.m_geometry_x = m_start_point.x();
        mv.define_bit(ModalVariable::GeometryXBit);
        writer.write_int(mv.m_geometry_x);
    }

    if(info_byte & HasY)
    {
        mv.m_geometry_y = m_start_point.y();
        mv.define_bit(ModalVariable::GeometryYBit);
        writer.write_int(mv.m_geometry_y);
    }

    if(info_byte & HasRepetition)
    {
        write_repetition(writer);
    }

    //TODO write element properties, do not support
}

bool PathElement::compare(OasisElement* rh)
{
    PathElement* b = dynamic_cast<PathElement*>(rh);
    oas_assert(b != NULL);
    if(m_halfwidth != b->m_halfwidth)
    {
        return m_halfwidth < b->m_halfwidth;
    }
    if(m_start_ext != b->m_start_ext)
    {
        return m_start_ext < b->m_start_ext;
    }
    if(m_end_ext != b->m_end_ext)
    {
        return m_end_ext < b->m_end_ext;
    }
    if(m_point_list != b->m_point_list)
    {
        return m_point_list < b->m_point_list;
    }
    if(m_start_point != b->m_start_point)
    {
        return m_start_point < b->m_start_point;
    }
    return this < rh;
}

bool PathElement::equal(OasisElement* rh)
{
    PathElement* b = dynamic_cast<PathElement*>(rh);
    oas_assert(b != NULL);
    return m_halfwidth == b->m_halfwidth
        && m_start_ext == b->m_start_ext
        && m_end_ext == b->m_end_ext
        && m_point_list == b->m_point_list;
}

void TextElement::create(int64 x, int64 y, const OasisTextString& textstring, uint64 textlayer, uint64 texttype, OasisRepetition* repetition)
{
    m_start_point.set_x(x);
    m_start_point.set_y(y);
    m_textstring = textstring;
    if(repetition)
    {
        set_repetition(*repetition);
    }
}

OasisElement* TextElement::clone()
{
    TextElement* e = new TextElement;
    e->set_base(this);
    e->m_textstring = m_textstring;
    e->m_start_point = m_start_point;
    return e;
}

void TextElement::bbox(Box& box)
{
    box.set_left(m_start_point.x());
    box.set_bottom(m_start_point.y());
    box.set_right(m_start_point.x());
    box.set_top(m_start_point.y());
}

void TextElement::write(OasisWriter& writer)
{
    using namespace TextInfoByte;

    ModalVariable& mv = writer.get_modal_variables();
    uint8 info_byte = 0;

    info_byte |= HasRefnum; //TODO check

    if(!mv.defined_bit(ModalVariable::TextStringBit) || mv.m_textstring != m_textstring)
    {
        info_byte |= HasExplicitText;
    }

    if(!mv.defined_bit(ModalVariable::TextLayerBit) || mv.m_textlayer != m_layer)
    {
        info_byte |= HasTextlayer;
    }

    if(!mv.defined_bit(ModalVariable::TextTypeBit) || mv.m_texttype != m_datatype)
    {
        info_byte |= HasTexttype;
    }

    if(mv.m_text_x != m_start_point.x())
    {
        info_byte |= HasX;
    }

    if(mv.m_text_y != m_start_point.y())
    {
        info_byte |= HasY;
    }

    if(has_repetition())
    {
        info_byte |= HasRepetition;
    }

    writer.write_record_id(ORID_Text);
    writer.write_byte(info_byte);

    if(info_byte & HasExplicitText)
    {
        mv.m_textstring = m_textstring;
        mv.define_bit(ModalVariable::TextStringBit);
        writer.write_uint(m_textstring.get_index()); //TODO use refnum or textstring, double check
    }

    if(info_byte & HasTextlayer)
    {
        mv.m_textlayer = m_layer;
        mv.define_bit(ModalVariable::TextLayerBit);
        writer.write_uint(mv.m_textlayer);
    }

    if(info_byte & HasTexttype)
    {
        mv.m_texttype = m_datatype;
        mv.define_bit(ModalVariable::TextTypeBit);
        writer.write_uint(mv.m_texttype);
    }

    if(info_byte & HasX)
    {
        mv.m_text_x = m_start_point.x();
        mv.define_bit(ModalVariable::TextXBit);
        writer.write_int(mv.m_text_x);
    }

    if(info_byte & HasY)
    {
        mv.m_text_y = m_start_point.y();
        mv.define_bit(ModalVariable::TextYBit);
        writer.write_int(mv.m_text_y);
    }

    if(info_byte & HasRepetition)
    {
        write_repetition(writer);
    }

    //write element properties
}

void PlacementElement::create(int64 infoindex, const OasisTrans& trans, OasisRepetition* repetition)
{
    m_infoindex = infoindex;
    m_trans = trans;

    if(repetition)
    {
        set_repetition(*repetition);
    }
}

OasisElement* PlacementElement::clone()
{
    PlacementElement* e = new PlacementElement;
    e->set_base(this);
    e->m_infoindex = m_infoindex;
    e->m_trans = m_trans;
    return e;
}

void PlacementElement::write(OasisWriter& writer)
{
    using namespace PlacementInfoByte;

    ModalVariable& mv = writer.get_modal_variables();
    uint8 info_byte = 0;

    info_byte |= HasRefnum;

    if(!mv.defined_bit(ModalVariable::PlacementCellBit) || mv.m_placement_cell != m_infoindex)
    {
        info_byte |= HasExplicitCell;
    }

    if(mv.m_placement_x != m_trans.disp().x())
    {
        info_byte |= HasX;
    }

    if(mv.m_placement_y != m_trans.disp().y())
    {
        info_byte |= HasY;
    }

    if(has_repetition())
    {
        info_byte |= HasRepetition;
    }

    if(m_trans.is_mirror())
    {
        info_byte |= HasMirror;
    }

    if(m_trans.is_complex())
    {
        writer.write_record_id(ORID_PlacementTrans);
        info_byte |= AngleMask;
        writer.write_byte(info_byte);
    }
    else
    {
        writer.write_record_id(ORID_Placement);
        info_byte |= ((m_trans.rotation() << 1) & AngleMask);//assume rot is 0,1,2,3
        writer.write_byte(info_byte);
    }

    if(info_byte & HasExplicitCell)
    {
        oas_assert(info_byte & HasRefnum);
        mv.m_placement_cell = m_infoindex;
        mv.define_bit(ModalVariable::PlacementCellBit);
        writer.write_uint(m_infoindex);
    }

    if(m_trans.is_complex())
    {
        writer.write_real(OasisReal(m_trans.mag()));
        writer.write_real(OasisReal(m_trans.angle()));
    }

    if(info_byte & HasX)
    {
        mv.m_placement_x = m_trans.disp().x();
        mv.define_bit(ModalVariable::PlacementXBit);
        writer.write_int(mv.m_placement_x);
    }

    if(info_byte & HasY)
    {
        mv.m_placement_y = m_trans.disp().y();
        mv.define_bit(ModalVariable::PlacementYBit);
        writer.write_int(mv.m_placement_y);
    }

    if(has_repetition())
    {
        write_repetition(writer);
    }

    //write element properties
}

Box PlacementElement::get_bbox(OasisLayout& layout) const
{
    OasisCellInfo& info = layout.get_cell_info(get_info_index());
    OasisCell& cell = layout.get_cell(info.get_cell_index());
    Box box = cell.get_bbox();
    box.trans(get_trans());
    return box;
}

Box PlacementElement::get_bbox_with_repetition(OasisLayout& layout) const
{
    if (!has_repetition())
    {
        return get_bbox(layout);
    }

    Box box0 = get_bbox(layout);
    Box rbox;
    m_repetition->get_bbox(rbox);
    return Box(rbox.left() + box0.left(), rbox.bottom() + box0.bottom(), rbox.right() + box0.right(), rbox.top() + box0.top());
}

Box PlacementElement::get_bbox_with_repetition(const Box& box) const
{
    if (!has_repetition())
    {
        return box;
    }
    Box rbox;
    m_repetition->get_bbox(rbox);
    return Box(rbox.left() + box.left(), rbox.bottom() + box.bottom(), rbox.right() + box.right(), rbox.top() + box.top());
}

void OasisCell::add_geometry(OasisElement* e)
{
    if (!e->valid())
    {
        delete e;
        return;
    }

    m_geometries.push_back(e);
}

void OasisCell::add_text(OasisElement* e)
{
    if (!e->valid())
    {
        delete e;
        return;
    }

    m_texts.push_back(e);
}

void OasisCell::add_instance(OasisElement* e)
{
    if (!e->valid())
    {
        delete e;
        return;
    }

    m_instances.push_back(e);
}

void OasisCell::compute_bbox(OasisLayout& layout)
{
    if(m_boxready)
    {
        return;
    }

    OAS_DBG << "compute bbox " << get_infoindex() << OAS_ENDL;

    bool temp_load = false;
    if(!m_cached)
    {
        load(layout);
        temp_load = true;
    }

    for(uint32 i = 0; i < m_geometries.size(); ++i)
    {
        OasisElement* e = m_geometries[i];
        Box box;
        e->get_bbox_with_repetition(box);
        m_bbox.update(box);
    }
    for(uint32 i = 0; i < m_texts.size(); ++i)
    {
        TextElement* t = dynamic_cast<TextElement*>(m_texts[i]);
        Box box;
        t->get_bbox_with_repetition(box);
        m_bbox.update(box);
    }
    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(m_instances[i]);
        oas_assert(p != NULL);
        OasisCellInfo& info = layout.get_cell_info(p->get_info_index());
        OasisCell& cell = layout.get_cell(info.get_cell_index());
        cell.compute_bbox(layout);

        Box box = p->get_bbox_with_repetition(layout);
        m_bbox.update(box);
    }

    if (temp_load)
    {
        unload(layout);
    }

    m_boxready = true;
    OAS_DBG << "boxready " << get_infoindex() << OAS_ENDL;
}

bool OasisCell::try_compute_bbox(OasisLayout& layout)
{
    if(m_boxready)
    {
        return true;
    }

    OAS_DBG << "try compute bbox " << get_infoindex() << OAS_ENDL;

    bool temp_load = false;
    if(!m_cached)
    {
        if(!m_saved)
        {
            return false;
        }
        load(layout);
        temp_load = true;
    }

    if(!m_geomboxready)
    {
        for(uint32 i = 0; i < m_geometries.size(); ++i)
        {
            OasisElement* e = m_geometries[i];
            Box box;
            e->get_bbox_with_repetition(box);
            m_bbox.update(box);
        }
        for(uint32 i = 0; i < m_texts.size(); ++i)
        {
            TextElement* t = dynamic_cast<TextElement*>(m_texts[i]);
            Box box;
            t->get_bbox_with_repetition(box);
            m_bbox.update(box);
        }
        m_geomboxready = true;
    }

    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(m_instances[i]);
        oas_assert(p != NULL);
        OasisCellInfo& info = layout.get_cell_info(p->get_info_index());
        if(!info.is_defined())
        {
            return false;
        }
        OasisCell& cell = layout.get_cell(info.get_cell_index());
        if(!cell.try_compute_bbox(layout))
        {
            return false;
        }

        Box box = p->get_bbox_with_repetition(layout);
        m_bbox.update(box);
    }

    if (temp_load)
    {
        unload(layout);
    }

    m_boxready = true;
    OAS_DBG << "box ready " << get_infoindex() << OAS_ENDL;
    return true;
}

void OasisCell::update_layers(OasisLayout& layout)
{
    if (m_layers_ready)
    {
        return;
    }

    OAS_DBG << "update layers " << get_infoindex() << OAS_ENDL;

    bool temp_load = false;
    if(!m_cached)
    {
        load(layout);
        temp_load = true;
    }

    if(m_geometries.size() + m_texts.size() > 0)
    {
        std::set<std::pair<int32, int32> > layers;
        for(uint32 i = 0; i < m_geometries.size(); ++i)
        {
            OasisElement* e = m_geometries[i];
            layers.insert(std::make_pair(e->layer(), e->datatype()));
        }
        for(uint32 i = 0; i < m_texts.size(); ++i)
        {
            TextElement* t = dynamic_cast<TextElement*>(m_texts[i]);
            layers.insert(std::make_pair(t->layer(), t->datatype()));
        }
        layout.update_layers(layers);
    }

    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(m_instances[i]);
        oas_assert(p != NULL);
        OasisCellInfo& info = layout.get_cell_info(p->get_info_index());
        OasisCell& cell = layout.get_cell(info.get_cell_index());
        cell.update_layers(layout);
    }

    if (temp_load)
    {
        unload(layout);
    }

    m_layers_ready = true;
    OAS_DBG << "layers ready " << get_infoindex() << OAS_ENDL;
}

bool OasisCell::try_update_layers(OasisLayout& layout)
{
    if (m_layers_ready)
    {
        return true;
    }

    OAS_DBG << "try update layers " << get_infoindex() << OAS_ENDL;

    bool temp_load = false;
    if(!m_cached)
    {
        load(layout);
        temp_load = true;
    }

    if(m_geometries.size() + m_texts.size() > 0)
    {
        std::set<std::pair<int32, int32> > layers;
        for(uint32 i = 0; i < m_geometries.size(); ++i)
        {
            OasisElement* e = m_geometries[i];
            layers.insert(std::make_pair(e->layer(), e->datatype()));
        }
        for(uint32 i = 0; i < m_texts.size(); ++i)
        {
            TextElement* t = dynamic_cast<TextElement*>(m_texts[i]);
            layers.insert(std::make_pair(t->layer(), t->datatype()));
        }
        layout.update_layers(layers);
    }

    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(m_instances[i]);
        oas_assert(p != NULL);
        OasisCellInfo& info = layout.get_cell_info(p->get_info_index());
        if (!info.is_defined())
        {
            return false;
        }
        OasisCell &cell = layout.get_cell(info.get_cell_index());
        if(!cell.try_update_layers(layout))
        {
            return false;
        }
    }

    if (temp_load)
    {
        unload(layout);
    }

    m_layers_ready = true;
    OAS_DBG << "layers ready " << get_infoindex() << OAS_ENDL;
    return true;
}

void OasisCell::collect_children(OasisLayout& layout)
{
    if (m_children_ready)
    {
        return;
    }

    OAS_DBG << "collect children " << get_infoindex() << OAS_ENDL;

    bool temp_load = false;
    if(!m_cached)
    {
        load(layout);
        temp_load = true;
    }

    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(m_instances[i]);
        oas_assert(p != NULL);
        m_children_infos.insert(p->get_info_index());
    }

    if (temp_load)
    {
        unload(layout);
    }

    m_children_ready = true;
    OAS_DBG << "collect children " << get_infoindex() << OAS_ENDL;
}

void OasisCell::collect_parents(OasisLayout& layout)
{
    if (!m_children_ready)
    {
        OAS_INFO << "children not collected " << get_infoindex() << OAS_ENDL;
        return;
    }
    for (auto it = m_children_infos.begin(); it != m_children_infos.end(); ++it)
    {
        OasisCellInfo& info = layout.get_cell_info(*it);
        OasisCell &cell = layout.get_cell(info.get_cell_index());
        cell.m_parents_infos.insert(get_infoindex());
    }
}

void OasisCell::print_children()
{
    std::stringstream ss;
    ss << get_infoindex() << " children ";
    for (auto it = m_children_infos.begin(); it != m_children_infos.end(); ++it)
    {
        ss << *it << ",";
    }
    OAS_INFO << ss.str() << OAS_ENDL;
}

void OasisCell::print_parents()
{
    std::stringstream ss;
    ss << get_infoindex() << " parents ";
    for (auto it = m_parents_infos.begin(); it != m_parents_infos.end(); ++it)
    {
        ss << *it << ",";
    }
    OAS_INFO << ss.str() << OAS_ENDL;
}

void OasisCell::compute_layer_bbox(OasisLayout &layout, const std::set<LDType> &layers, std::map<int64, std::map<LDType, Box> >& result)
{
    if (result.count(get_infoindex()))
    {
        return;
    }

    OAS_DBG << "compute layer bbox " << get_infoindex() << OAS_ENDL;

    bool temp_load = false;
    if(!m_cached)
    {
        load(layout);
        temp_load = true;
    }

    std::map<LDType, Box> &box_map = result[get_infoindex()];
    for(uint32 i = 0; i < m_geometries.size(); ++i)
    {
        OasisElement* e = m_geometries[i];
        LDType ld(e->layer(), e->datatype());
        if (layers.count(ld))
        {
            Box box;
            e->get_bbox_with_repetition(box);
            box_map[ld].update(box);
        }
    }
    for(uint32 i = 0; i < m_texts.size(); ++i)
    {
        TextElement* t = dynamic_cast<TextElement*>(m_texts[i]);
        LDType ld(t->layer(), t->datatype());
        if (layers.count(ld))
        {
            Box box;
            t->get_bbox_with_repetition(box);
            box_map[ld].update(box);
        }
    }
    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(m_instances[i]);
        oas_assert(p != NULL);
        if (result.count(p->get_info_index()) == 0)
        {
            OasisCellInfo &info = layout.get_cell_info(p->get_info_index());
            OasisCell& cell = layout.get_cell(info.get_cell_index());
            cell.compute_layer_bbox(layout, layers, result);
        }

        for (auto it = layers.begin(); it != layers.end(); ++it)
        {
            const LDType& ld = *it;
            Box box0 = result[p->get_info_index()][ld];
            box0.trans(p->get_trans());
            Box box = p->get_bbox_with_repetition(box0);
            box_map[ld].update(box);
        }
    }

    if (temp_load)
    {
        unload(layout);
    }

    OAS_DBG << "compute layer bbox " << get_infoindex() << OAS_ENDL;
}

void OasisCell::assign_raw_data(OasisLayout &layout, const Box &bbox, const std::vector<std::pair<int32, int32> > &layers, const uint8 *data, int32 data_size)
{
    m_bbox = bbox;
    m_boxready = true;

    for (auto it = layers.begin(); it != layers.end(); ++it)
    {
        layout.insert_layer(it->first, it->second);
    }
    m_layers_ready = true;

    OasisWriter &writer = layout.writer();
    m_fileoffset = writer.get_offset();
    writer.write_record_id(ORID_CellRef);
    writer.write_uint(m_infoindex);
    writer.write_bytes(data, data_size);
    m_hint_datasize = writer.get_offset() - m_fileoffset + 1; //read over 1 byte
    m_saved = true;

    m_cached = false;
}

void OasisCell::partition(OasisLayout& layout)
{
    oas_assert(!m_saved);
    OAS_DBG << "partition " << get_infoindex() << " geom " << m_geometries.size() << " text " << m_texts.size() << " inst " << m_instances.size() << OAS_ENDL;
    if(m_geometries.size() + m_texts.size() + m_instances.size() == 0)
    {
        return;
    }
    else if(m_geometries.size() + m_texts.size() > 0 && !m_instances.empty())
    {
        //separated geometry cell might have bad dimension
        //partition the original instances first
        partition_instance(layout);
        separate_geometry(layout);
    }
    else if(m_geometries.size() + m_texts.size() > 0)
    {
        //partition geometry might create much instances
        //do partition instance again
        partition_geometry(layout);
        partition_instance(layout);
    }
    else
    {
        partition_instance(layout);
    }
}

void OasisCell::partition_instance(OasisLayout& layout)
{
    oas_assert(!m_saved);
    if(m_instances.size() < layout.instance_count_limit())
    {
        return;
    }
    std::vector<PlacementElement*> complete_instances;
    std::vector<PlacementElement*> uncomplete_instances;
    complete_instances.reserve(m_instances.size());
    uncomplete_instances.reserve(m_instances.size());
    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = static_cast<PlacementElement*>(m_instances[i]);
        OasisCellInfo& info = layout.get_cell_info(p->get_info_index());
        if(info.is_defined())
        {
            OasisCell& cell = layout.get_cell(info.get_cell_index());
            if(cell.try_compute_bbox(layout))
            {
                OAS_DBG << "complete instance" << OAS_ENDL;
                complete_instances.push_back(p);
            }
            else
            {
                OAS_DBG << "uncomplete instance" << OAS_ENDL;
                uncomplete_instances.push_back(p);
            }
        }
        else
        {
            OAS_DBG << "uncomplete instance" << OAS_ENDL;
            uncomplete_instances.push_back(p);
        }
    }

    m_instances.clear();

    if(!uncomplete_instances.empty())
    {
        Box bbox;
        for(uint32 i = 0; i < uncomplete_instances.size(); ++i)
        {
            PlacementElement* p = uncomplete_instances[i];
            bbox.update(p->get_trans().disp());
        }
        bbox.set_right(bbox.right() + 1);
        bbox.set_top(bbox.top() + 1);
        uint64 cx = 1;
        uint64 cy = 1;
        while(cx * cy < uncomplete_instances.size() / layout.instance_count_limit())
        {
            if(bbox.width() / cx > bbox.height() / cy)
            {
                ++cx;
            }
            else
            {
                ++cy;
            }
        }

        int64 spanx = (bbox.width() + cx - 1) / cx;
        int64 spany = (bbox.height() + cy - 1) / cy;
        std::vector<std::vector<PlacementElement*> > grid_instances(cx * cy);
        for(uint32 i = 0; i < uncomplete_instances.size(); ++i)
        {
            PlacementElement* p = uncomplete_instances[i];
            Point pt = p->get_trans().disp();
            int64 c = (pt.x() - bbox.left()) / spanx;
            int64 r = (pt.y() - bbox.bottom()) / spany;
            grid_instances[r * cx + c].push_back(uncomplete_instances[i]);
        }

        for(uint32 i = 0; i < grid_instances.size(); ++i)
        {
            if(grid_instances[i].empty())
            {
                continue;
            }
            stringstream ss;
            ss << generate_cellname_prefix << "uncomp_grid_" << get_infoindex() << "_" << i;
            int64 cellindex = layout.create_cell();
            int64 infoindex = layout.create_cell_info(-1, ss.str());
            OasisCell& cell = layout.get_cell(cellindex);
            OasisCellInfo& info = layout.get_cell_info(infoindex);
            cell.set_infoindex(infoindex);
            info.set_cell_index(cellindex);
            info.set_referenced();
            PlacementElement* p = new PlacementElement;
            p->create(infoindex, OasisTrans());
            add_instance(p);
            for(uint32 k = 0; k < grid_instances[i].size(); ++k)
            {
                cell.add_instance(grid_instances[i][k]);
            }
            cell.try_compute_bbox(layout);
            cell.try_update_layers(layout);
            cell.unload(layout);
        }
    }

    Box bbox;
    for(uint32 i = 0; i < complete_instances.size(); ++i)
    {
        PlacementElement* p = complete_instances[i];
        bbox.update(p->get_bbox(layout));
    }

    uint64 cx = 1;
    uint64 cy = 1;
    while(cx * cy < complete_instances.size() / layout.instance_count_limit())
    {
        if(bbox.width() / cx > bbox.height() / cy)
        {
            ++cx;
        }
        else
        {
            ++cy;
        }
    }

    int64 spanx = (bbox.width() + cx - 1) / cx;
    int64 spany = (bbox.height() + cy - 1) / cy;
    std::vector<std::vector<PlacementElement*> > grid_instances(cx * cy);
    for(uint32 i = 0; i < complete_instances.size(); ++i)
    {
        PlacementElement* p = complete_instances[i];
        Box box = p->get_bbox(layout);
        int64 c = (box.left() - bbox.left()) / spanx;
        int64 r = (box.bottom() - bbox.bottom()) / spany;
        grid_instances[r * cx + c].push_back(complete_instances[i]);
    }

    for(uint32 i = 0; i < grid_instances.size(); ++i)
    {
        if(grid_instances[i].empty())
        {
            continue;
        }
        stringstream ss;
        ss << generate_cellname_prefix << "comp_grid_" << get_infoindex() << "_" << i;
        int64 cellindex = layout.create_cell();
        int64 infoindex = layout.create_cell_info(-1, ss.str());
        OasisCell& cell = layout.get_cell(cellindex);
        OasisCellInfo& info = layout.get_cell_info(infoindex);
        cell.set_infoindex(infoindex);
        info.set_cell_index(cellindex);
        info.set_referenced();
        PlacementElement* p = new PlacementElement;
        p->create(infoindex, OasisTrans());
        add_instance(p);
        for(uint32 k = 0; k < grid_instances[i].size(); ++k)
        {
            cell.add_instance(grid_instances[i][k]);
        }
        cell.compute_bbox(layout);
        cell.update_layers(layout);
        cell.unload(layout);
    }
}

void OasisCell::separate_geometry(OasisLayout& layout)
{
    oas_assert(!m_saved);
    if(m_geometries.size() + m_texts.size() == 0)
    {
        return;
    }
    if(m_instances.empty())
    {
        partition_geometry(layout);
        partition_instance(layout);
        return;
    }
    OAS_DBG << "separate geometry " << get_infoindex() << ",geom " << m_geometries.size() << ",text " << m_texts.size() << ",inst " << m_instances.size() << OAS_ENDL;
    int64 cellindex = layout.create_cell();
    std::stringstream ss;
    ss << generate_cellname_prefix << "geom_sp_" << get_infoindex();
    int64 infoindex = layout.create_cell_info(-1, ss.str());
    OasisCell& cell = layout.get_cell(cellindex);
    cell.set_infoindex(infoindex);
    OasisCellInfo& info = layout.get_cell_info(infoindex);
    info.set_cell_index(cellindex);
    info.set_referenced();
    for(uint32 i = 0; i < m_geometries.size(); ++i)
    {
        cell.add_geometry(m_geometries[i]);
    }
    m_geometries.clear();
    for(uint32 i = 0; i < m_texts.size(); ++i)
    {
        cell.add_text(m_texts[i]);
    }
    m_geometries.clear();
    PlacementElement* e = new PlacementElement;
    e->create(infoindex, OasisTrans());
    add_instance(e);

    cell.partition_geometry(layout);
    cell.partition_instance(layout);

    cell.compute_bbox(layout);
    cell.update_layers(layout);
    cell.unload(layout);
}

void OasisCell::partition_geometry(OasisLayout& layout)
{
    oas_assert(!m_saved);
    OAS_DBG << "partition geometry " << get_infoindex() << " geom " << m_geometries.size() << " text " << m_texts.size() << " inst " << m_instances.size() << OAS_ENDL;
    compute_bbox(layout);
    if (m_geometries.size() + m_texts.size() < layout.geometry_count_limit()
        && get_bbox().width() < layout.max_geometry_cell_dimension()
        && get_bbox().height() < layout.max_geometry_cell_dimension())
    {
        return;
    }

    Box bbox;
    std::vector<Box> geomBox(m_geometries.size());
    for(uint32 i = 0; i < m_geometries.size(); ++i)
    {
        OasisElement* e = m_geometries[i];
        e->get_bbox_with_repetition(geomBox[i]);
        if(geomBox[i].width() < layout.max_geometry_dimension() && geomBox[i].height() < layout.max_geometry_dimension())
        {
            bbox.update(geomBox[i]);
        }
    }
    std::vector<Box> textBox(m_texts.size());
    for(uint32 i = 0; i < m_texts.size(); ++i)
    {
        OasisElement* e = m_geometries[i];
        e->get_bbox_with_repetition(textBox[i]);
        bbox.update(textBox[i]);
    }

    int64 count = (m_geometries.size() + m_texts.size() + layout.geometry_count_limit() - 1) / layout.geometry_count_limit();
    int64 cx = 1;
    int64 cy = 1;
    while (cx * cy < count || bbox.width() > cx * layout.max_geometry_cell_dimension() || bbox.height() > cy * layout.max_geometry_cell_dimension())
    {
        if(bbox.width() / cx > bbox.height() / cy)
        {
            ++cx;
        }
        else
        {
            ++cy;
        }
    }
    int64 spanx = (bbox.width() + cx - 1) / cx;
    int64 spany = (bbox.height() + cy - 1) / cy;

    OAS_DBG << "cx " << cx << ",cy " << cy << ",spanx " << spanx << ",spany " << spany << OAS_ENDL;

    std::vector<std::vector<OasisElement*> > geometries(cx * cy);
    std::vector<std::vector<OasisElement*> > texts(cx * cy);
    std::vector<OasisElement*> biggeometries;
    Box big_bbox;
    std::vector<Box> big_geom_box;
    for(uint32 i = 0; i < m_geometries.size(); ++i)
    {
        if(geomBox[i].width() < layout.max_geometry_dimension() && geomBox[i].height() < layout.max_geometry_dimension())
        {
            int64 c = ((geomBox[i].left() + geomBox[i].right()) / 2 - bbox.left()) / spanx;
            int64 r = ((geomBox[i].bottom() + geomBox[i].top()) / 2 - bbox.bottom()) / spany;
            c = std::min(c, cx-1);
            c = std::max(c, (int64)0);
            r = std::min(r, cy-1);
            r = std::max(r, (int64)0);
            geometries[r * cx + c].push_back(m_geometries[i]);
        }
        else
        {
            biggeometries.push_back(m_geometries[i]);
            big_bbox.update(geomBox[i]);
            big_geom_box.push_back(geomBox[i]);
        }
    }
    for(uint32 i = 0; i < m_texts.size(); ++i)
    {
        int64 c = (textBox[i].left() - bbox.left()) / spanx;
        int64 r = (textBox[i].bottom() - bbox.bottom()) / spany;
        texts[r * cx + c].push_back(m_texts[i]);
    }
    m_geometries.clear();
    m_texts.clear();

    for(uint32 i = 0; i < geometries.size(); ++i)
    {
        OAS_DBG << "grid " << i << ",geom " << geometries[i].size() << ",text " << texts[i].size() << OAS_ENDL;
        if(geometries[i].size() + texts[i].size() == 0)
        {
            continue;
        }
        stringstream ss;
        ss << generate_cellname_prefix << "geom_part_" << get_infoindex() << "_" << i;
        int64 cellindex = layout.create_cell();
        int64 infoindex = layout.create_cell_info(-1, ss.str());
        OasisCell& cell = layout.get_cell(cellindex);
        OasisCellInfo& info = layout.get_cell_info(infoindex);
        cell.set_infoindex(infoindex);
        info.set_cell_index(cellindex);
        info.set_referenced();
        PlacementElement* e = new PlacementElement;
        e->create(infoindex, OasisTrans());
        add_instance(e);
        for(uint32 k = 0; k < geometries[i].size(); ++k)
        {
            cell.add_geometry(geometries[i][k]);
        }
        for(uint32 k = 0; k < texts[i].size(); ++k)
        {
            cell.add_text(texts[i][k]);
        }
        cell.compute_bbox(layout);
        cell.update_layers(layout);
        cell.unload(layout);
    }

    if (biggeometries.empty())
    {
        return;
    }

    OAS_DBG << "big geom " << biggeometries.size() << OAS_ENDL;
    count = (biggeometries.size() + layout.large_geometry_count_limit() - 1) / layout.large_geometry_count_limit();
    cx = 1;
    cy = 1;
    while (cx * cy < count)
    {
        if(big_bbox.width() / cx > big_bbox.height() / cy)
        {
            ++cx;
        }
        else
        {
            ++cy;
        }
    }
    spanx = (big_bbox.width() + cx - 1) / cx;
    spany = (big_bbox.height() + cy - 1) / cy;

    std::vector<std::vector<OasisElement*> > big_grids(cx * cy);
    for (uint32 i = 0; i < biggeometries.size(); ++i)
    {
        int64 c = ((big_geom_box[i].left() + big_geom_box[i].right()) / 2 - bbox.left()) / spanx;
        int64 r = ((big_geom_box[i].bottom() + big_geom_box[i].top()) / 2 - bbox.bottom()) / spany;
        c = std::min(c, cx-1);
        c = std::max(c, (int64)0);
        r = std::min(r, cy-1);
        r = std::max(r, (int64)0);
        big_grids[r * cx + c].push_back(biggeometries[i]);
    }
    biggeometries.clear();
    for (uint32 i = 0; i < big_grids.size(); ++i)
    {
        if (big_grids.empty())
        {
            continue;
        }

        OAS_DBG << "big grid " << i << ", geom " << big_grids[i].size() << OAS_ENDL;
        stringstream ss;
        ss << generate_cellname_prefix << "geom_bigpart_" << get_infoindex() << "_" << i;
        int64 cellindex = layout.create_cell();
        int64 infoindex = layout.create_cell_info(-1, ss.str());
        OasisCell& cell = layout.get_cell(cellindex);
        OasisCellInfo& info = layout.get_cell_info(infoindex);
        cell.set_infoindex(infoindex);
        info.set_cell_index(cellindex);
        info.set_referenced();
        PlacementElement* e = new PlacementElement;
        e->create(infoindex, OasisTrans());
        add_instance(e);
        for (uint32 k = 0; k < big_grids[i].size(); ++k)
        {
            cell.add_geometry(big_grids[i][k]);
        }
        cell.compute_bbox(layout);
        cell.update_layers(layout);
        cell.unload(layout);
    }
}

void OasisCell::flush(OasisLayout& layout)
{
    if(!m_saved)
    {
        OasisWriter& writer = layout.writer();
        if (!layout.multiple_thread_write())
        {
            m_fileoffset = writer.get_offset();
            write(writer);
            m_hint_datasize = writer.get_offset() - m_fileoffset + 1; //read over 1 byte
            m_saved = true;
            OAS_DBG << "flush " << get_infoindex() << " offset " << m_fileoffset << OAS_ENDL;
        }
        else
        {
            MemoryOasisWriter memory_writer(writer);
            write(memory_writer);

            writer.get_locker().Lock();
            m_fileoffset = writer.get_offset();
            writer.write_bytes(memory_writer.get_data(), memory_writer.get_size());
            writer.get_locker().Unlock();

            m_hint_datasize = memory_writer.get_size() + 1; //read over 1 byte
            m_saved = true;
            OAS_DBG << "flush " << get_infoindex() << " offset " << m_fileoffset << OAS_ENDL;
        }
    }
}

void OasisCell::load(OasisLayout& layout)
{
    if (m_import_mode)
    {
        sys::MutexGuard guard(m_locker);
        import_load(layout);
    }
    else
    {
        parse_load(layout);
    }
}

void OasisCell::parse_load(OasisLayout& layout)
{
    if(m_cached)
    {
        return;
    }
    oas_assert(m_saved);

    OAS_DBG << "parse load cell " << get_infoindex() << OAS_ENDL;

    layout.flush_writer(layout.writer());

    OasisParser parser;
    parser.open_file(layout.get_file_name());
    parser.set_file_offset(m_fileoffset, m_hint_datasize);
    parser.load_cell(&layout, m_infoindex);
    m_cached = true;
}

void OasisCell::import_load(OasisLayout& layout)
{
    if (m_cached)
    {
        return;
    }

    OAS_DBG << "import load cell " << get_infoindex() << OAS_ENDL;
    OAS_DBG << "offset " << m_fileoffset << ", size " << m_hint_datasize << OAS_ENDL;

    OasisParser parser;
    parser.open_file(layout.get_file_name());
    parser.set_file_offset(m_fileoffset, m_hint_datasize);
    parser.load_cell(&layout, m_infoindex);
    m_cached = true;
}

void OasisCell::cleanup(bool force)
{
    if(!m_cached)
    {
        return;
    }
    if (m_stick && !force)
    {
        return;
    }
    m_cached = false;
    for(uint32 i = 0; i < m_geometries.size(); ++i)
    {
        OasisElement* e = m_geometries[i];
        delete e;
    }
    m_geometries.clear();
    for(uint32 i = 0; i < m_texts.size(); ++i)
    {
        OasisElement* t = m_texts[i];
        delete t;
    }
    m_texts.clear();
    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        OasisElement* p = m_instances[i];
        delete p;
    }
    m_instances.clear();

    std::vector<OasisElement*>().swap(m_geometries);
    std::vector<OasisElement*>().swap(m_texts);
    std::vector<OasisElement*>().swap(m_instances);
}

uint64 OasisCell::count_objects()
{
    if(!m_cached)
    {
        return 0;
    }
    return m_geometries.size() +  m_texts.size() + m_instances.size();
}

void OasisCell::clone(OasisCell& dest, OasisLayout& layout, int64 infoindex)
{
    load(layout);
    dest.cleanup();
    dest.m_infoindex = infoindex;
    dest.m_fileoffset = -1;
    dest.m_saved = false;

    for(uint32 i = 0; i < m_geometries.size(); ++i)
    {
        OasisElement* e = m_geometries[i];
        dest.m_geometries.push_back(e->clone());
    }
    for(uint32 i = 0; i < m_texts.size(); ++i)
    {
        OasisElement* t = m_texts[i];
        dest.m_texts.push_back(t->clone());
    }
    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        OasisElement* p = m_instances[i];
        dest.m_instances.push_back(p->clone());
    }
    unload(layout);
    dest.m_cached = true;
    dest.m_bbox = m_bbox;
    dest.m_boxready = m_boxready;
    dest.m_geomboxready = m_geomboxready;
    dest.m_layers_ready = m_layers_ready;
}

void OasisCell::write(OasisWriter& writer)
{
    writer.write_record_id(ORID_CellRef);
    writer.write_uint(m_infoindex);
    OAS_DBG << "ref num " << m_infoindex << OAS_ENDL;

    compress();

    ModalVariable& mv = writer.get_modal_variables();
    mv.reset();

    writer.enter_cblock();

    for(std::vector<OasisElement*>::iterator it = m_geometries.begin(); it != m_geometries.end(); ++it)
    {
        OasisElement* p = *it;
        p->write(writer);
    }

    for(std::vector<OasisElement*>::iterator it = m_texts.begin(); it != m_texts.end(); ++it)
    {
        OasisElement* t = *it;
        t->write(writer);
    }

    for(std::vector<OasisElement*>::iterator it = m_instances.begin(); it != m_instances.end(); ++it)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(*it);
        p->write(writer);
    }

    writer.leave_cblock();
}

void OasisCell::unload(OasisLayout& layout)
{
    if (m_import_mode)
    {
        sys::MutexGuard guard(m_locker);
        if (unused())
        {
            cleanup();
        }
    }
    else
    {
        flush(layout);
        cleanup();
    }
    OAS_DBG << "unload cell " << get_infoindex() << OAS_ENDL;
}

void OasisCell::make_top_tree(OasisLayout& layout, const std::string& cellname, int level)
{
    if(m_instances.size() <= 16)
    {
        return;
    }
    std::vector<Box> instance_boxes;
    instance_boxes.resize(m_instances.size());
    Box topbox;
    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = static_cast<PlacementElement*>(m_instances[i]);
        instance_boxes[i] = p->get_bbox_with_repetition(layout);
        topbox.update(instance_boxes[i]);
    }

    int64 max_size = (int64)rint(100 / layout.get_dbu());
    uint64 max_count = 100;

    int64 cols = 1;
    int64 rows = 1;
    do
    {
        if(topbox.width() <= max_size && topbox.height() <= max_size && m_instances.size() <= max_count)
        {
            return;
        }

        if(topbox.width() > max_size && topbox.width() > 2 * topbox.height())
        {
            int64 span = std::max(max_size, topbox.height());
            cols = (topbox.width() + span - 1) / span;
            if(cols > 16) cols = 16;
            break;
        }

        if(topbox.height() > max_size && topbox.height() > 2 * topbox.width())
        {
            int64 span = std::max(max_size, topbox.width());
            rows = (topbox.height() + span - 1) / span;
            if(rows > 16) rows = 16;
            break;
        }

        if(cols * max_size < topbox.width())
        {
            cols = (topbox.width() + max_size - 1) / max_size;
            if(cols > 4) cols = 4;
        }

        if(rows * max_size < topbox.height())
        {
            rows = (topbox.height() + max_size - 1) / max_size;
            if(rows > 4) rows = 4;
        }

        if(cols > 1 || rows > 1)
        {
            break;
        }

        if(m_instances.size() > max_count)
        {
            cols = 2;
            rows = 2;
            break;
        }
        return;
    }while(0);

    int64 spanx = (topbox.width() + cols - 1) / cols;
    int64 spany = (topbox.height() + rows - 1) / rows;

    std::vector<std::vector<OasisElement*> > grids;
    std::vector<Box> grid_boxes;
    grids.resize(cols * rows);
    grid_boxes.resize(cols * rows);

    std::vector<OasisElement*> instances;
    instances.swap(m_instances);
    for(uint32 i = 0; i < instances.size(); ++i)
    {
        Box& box = instance_boxes[i];
        int64 cx = (box.left() + box.right()) / 2;
        int64 cy = (box.bottom() + box.top()) / 2;
        int64 col = (cx - topbox.left()) / spanx;
        int64 row = (cy - topbox.bottom()) / spany;
        int64 gindex = row * cols + col;
        grids[gindex].push_back(instances[i]);
        grid_boxes[gindex].update(box);
    }

    int nonempty = 0;
    for(uint32 i = 0; i < grids.size(); ++i)
    {
        if(!grids[i].empty())
        {
            nonempty++;
        }
    }
    if(nonempty <= 1)
    {//protect from infinite loop
        m_instances.swap(instances);

        logger::warning << "all instances put into one grid. cols " << cols << ", rows " << rows
            << ", spanx " << spanx << ", spany " << spany << ", #inst " << m_instances.size()
            << ", box (" << topbox.left() << "," << topbox.bottom() << "," << topbox.right() << "," << topbox.top() << ")";
        return;
    }

    std::set<uint64> cellset;
    for(uint32 i = 0; i < grids.size(); ++i)
    {
        if(grids[i].empty())
        {
            continue;
        }
        uint64 cellindex = layout.create_cell();
        int64 refnum = cellindex;
        std::stringstream ss;
        ss << cellname << "_" << level << "_" << cellindex;
        int64 cellinfoindex = layout.create_cell_info(refnum, ss.str());
        OasisCellInfo& cellinfo = layout.get_cell_info(cellinfoindex);
        cellinfo.set_referenced();
        cellinfo.set_cell_index(cellindex);
        OasisCell& cell = layout.get_cell(cellindex);
        cell.set_infoindex(cellinfoindex);
        for(uint32 j = 0; j < grids[i].size(); ++j)
        {
            cell.add_instance(grids[i][j]);
        }
        PlacementElement* p = new PlacementElement;
        p->create(cellinfoindex, OasisTrans());
        add_instance(p);
        cellset.insert(cellindex);
    }

    for(std::set<uint64>::iterator it = cellset.begin(); it != cellset.end(); ++it)
    {
        OasisCell& cell = layout.get_cell(*it);
        cell.make_top_tree(layout, cellname, level + 1);
    }
}


void OasisCell::make_top_tree_v2(OasisLayout& layout, const std::string& cellname, int level)
{
    if(m_instances.size() <= 32)
    {
        return;
    }
    std::vector<Box> instance_boxes;
    instance_boxes.resize(m_instances.size());
    Box topbox;
    for(uint32 i = 0; i < m_instances.size(); ++i)
    {
        PlacementElement* p = static_cast<PlacementElement*>(m_instances[i]);
        instance_boxes[i] = p->get_bbox_with_repetition(layout);
        topbox.update(instance_boxes[i]);
    }

    int64 max_size = (int64)rint(100 / layout.get_dbu());
    uint64 max_count = 100;
    if(topbox.width() <= max_size && topbox.height() <= max_size && m_instances.size() <= max_count)
    {
       return;
    }
        
    uint64 max_sub_fill = m_instances.size()/ 16 + 16;
    uint64 max_current_fill = m_instances.size();
    uint64 max_cut_count = 16;
    uint64 current_cut_count = 1;
    int64 min_size = (int64)rint(0.1 / layout.get_dbu());
    
    std::vector<OasisElement*> instances;
    instances.swap(m_instances);

    std::vector<std::vector<OasisElement*> > grids;
    int64 cols = 1;
    int64 rows = 1;
    int64 spanx = topbox.width();
    int64 spany = topbox.height();
    
    while ((max_current_fill > max_sub_fill) && (current_cut_count < max_cut_count))
    {
       spany > spanx ? rows *= 2 : cols *= 2;
   
       spanx = (topbox.width() + cols - 1) / cols;
       spany = (topbox.height() + rows - 1) / rows;
       if (spanx < min_size || spany < min_size) break;

       std::vector<std::vector<OasisElement*> > lgrids;
       lgrids.resize(cols * rows);

       for(uint32 i = 0; i < instances.size(); ++i)
       {
          Box& box = instance_boxes[i];
          int64 cx = (box.left() + box.right()) / 2;
          int64 cy = (box.bottom() + box.top()) / 2;
          int64 col = (cx - topbox.left()) / spanx;
          int64 row = (cy - topbox.bottom()) / spany;
          int64 gindex = row * cols + col;
          lgrids[gindex].push_back(instances[i]);
       }

       max_current_fill = 0;
       current_cut_count = 0;
       for(uint32 i = 0; i < lgrids.size(); ++i)
       {
          if(!lgrids[i].empty()) current_cut_count++;
          if (max_current_fill < lgrids[i].size()) max_current_fill = lgrids[i].size();
       }

       grids.swap(lgrids);
    }
    
    if(current_cut_count <= 1)
    {//protect from infinite loop
        m_instances.swap(instances);

        logger::warning << "all instances put into one grid. cols " << cols << ", rows " << rows
            << ", spanx " << spanx << ", spany " << spany << ", #inst " << m_instances.size()
            << ", box (" << topbox.left() << "," << topbox.bottom() << "," << topbox.right() << "," << topbox.top() << ")";
        return;
    }

    std::set<uint64> cellset;
    for(uint32 i = 0; i < grids.size(); ++i)
    {
        if(grids[i].empty())
        {
            continue;
        }
        uint64 cellindex = layout.create_cell();
        int64 refnum = cellindex;
        std::stringstream ss;
        ss << cellname << "_" << level << "_" << cellindex;
        int64 cellinfoindex = layout.create_cell_info(refnum, ss.str());
        OasisCellInfo& cellinfo = layout.get_cell_info(cellinfoindex);
        cellinfo.set_referenced();
        cellinfo.set_cell_index(cellindex);
        OasisCell& cell = layout.get_cell(cellindex);
        cell.set_infoindex(cellinfoindex);
        for(uint32 j = 0; j < grids[i].size(); ++j)
        {
            cell.add_instance(grids[i][j]);
        }
        PlacementElement* p = new PlacementElement;
        p->create(cellinfoindex, OasisTrans());
        add_instance(p);
        cellset.insert(cellindex);
    }

    for(std::set<uint64>::iterator it = cellset.begin(); it != cellset.end(); ++it)
    {
        OasisCell& cell = layout.get_cell(*it);
        cell.make_top_tree_v2(layout, cellname, level + 1);
    }
}


void OasisCell::compress()
{
    if(m_geometries.empty())
    {
        return;
    }
    std::vector<OasisElement*> geometries;
    geometries.reserve(m_geometries.size());

    std::sort(m_geometries.begin(), m_geometries.end(), compare_element);
    for(uint32 i = 0; i < m_geometries.size(); )
    {
        bool need_compress = false;
        uint32 j = i + 1;
        for(; j < m_geometries.size(); ++j)
        {
            if(equal_element(m_geometries[i], m_geometries[j]))
            {
                need_compress = true;
            }
            else
            {
                break;
            }
        }

        if(need_compress)
        {
            oas_assert(j > i + 1);
            OasisElement* e0 = m_geometries[i];
            Point p0;
            e0->offset(p0);
            std::vector<Point> disps0;
            e0->get_disps(disps0);
            for(uint32 k = i + 1; k < j; ++k)
            {
                OasisElement* e = m_geometries[k];
                Point p;
                e->offset(p);
                std::vector<Point> disps;
                e->get_disps(disps);
                Point shift = p - p0;
                for(uint32 n = 0; n < disps.size(); ++n)
                {
                    disps0.push_back(disps[n] + shift);
                }
            }

            OasisRepetition repetition;
            OasisRepetitionBuilder builder;
            builder.make_arbitory(repetition, disps0, 0, disps0.size());

            e0->set_repetition(repetition);
            geometries.push_back(e0);
            for(uint32 k = i + 1; k < j; ++k)
            {
                delete m_geometries[k];
            }
        }
        else
        {
            oas_assert(j == i + 1);
            geometries.push_back(m_geometries[i]);
        }

        i = j;
    }
    m_geometries.swap(geometries);
}

void OasisCell::accept_visitor(CellVisitor& visitor)
{
    OAS_DBG << "visit id " << get_infoindex() << OAS_ENDL;
    if(!visitor.query_box().overlap(get_bbox()))
    {
        return;
    }

    increase_use_count();
    update_use_time(visitor.get_layout()->get_time());

    if(!m_cached)
    {
        load(*visitor.get_layout());
    }

    visitor.set_info_index(get_infoindex());
    visitor.new_query_box(get_bbox());
    visitor.visit_geometries(m_geometries);
    visitor.visit_texts(m_texts);
    visitor.visit_instances(m_instances);

    decrease_use_count();
}

void OasisCellInfo::apply_properties(OasisLayout& layout)
{
    OasisCell &cell = layout.get_cell(get_cell_index());
    OasisPropertyBuilder builder;
    CellProperty cellprop;

    builder.decode_file_properties(layout, cellprop, m_properties);
    cell.set_bbox(cellprop.m_bbox);
    cell.set_offset(cellprop.m_offset);
    cell.set_hint_datasize(cellprop.m_cellsize);
    if(!cellprop.m_is_topcell)
    {
        set_referenced();
    }
    else
    {
        for (auto it = cellprop.m_layers.begin(); it != cellprop.m_layers.end(); ++it)
        {
            layout.insert_layer(it->first, it->second);
        }
    }

    std::vector<OasisProperty>().swap(m_properties);
}

void OasisCellInfo::get_properties(OasisLayout& layout, std::vector<OasisProperty>& properties)
{
    const OasisCell &cell = layout.get_cell(get_cell_index());
    OasisPropertyBuilder builder;
    CellProperty cellprop;
    cellprop.m_is_topcell = !is_referenced();
    cellprop.m_bbox = cell.get_bbox();
    cellprop.m_offset = cell.get_offset();
    cellprop.m_cellsize = cell.get_hint_datasize();
    if (cellprop.m_is_topcell)
    {
        layout.get_layers(cellprop.m_layers);
    }
    
    builder.encode_cell_properties(layout, cellprop, properties);
    OAS_DBG << "is top " << cellprop.m_is_topcell
        << "bbox " << cellprop.m_bbox.left() << "," << cellprop.m_bbox.bottom()
        << "," << cellprop.m_bbox.right() << "," << cellprop.m_bbox.top()
        << " offset " << cellprop.m_offset
        << " cellsize " << cellprop.m_cellsize
        << " #layers " << cellprop.m_layers.size() << OAS_ENDL;
}

void OasisTextString::write(OasisWriter& writer)
{
    writer.write_record_id(ORID_TextstringRef);
    writer.write_string(m_string);
    writer.write_uint(m_index);//TODO check
}

void OasisPropName::write(OasisWriter& writer)
{
    writer.write_record_id(ORID_PropnameRef);
    writer.write_string(m_string);
    writer.write_uint(m_index);//TODO use refnum
}

void OasisPropString::write(OasisWriter& writer)
{
    writer.write_record_id(ORID_PropstringRef);
    writer.write_string(m_string);
    writer.write_uint(m_index);//TODO check
}

void OasisLayerName::write(OasisWriter& writer)
{
    if(m_is_text_layer)
    {
        writer.write_record_id(ORID_LayernameText);
    }
    else
    {
        writer.write_record_id(ORID_Layername);
    }

    writer.write_string(m_layer_name);
    writer.write_interval(m_layer_interval);
    writer.write_interval(m_datatype_interval);
}

