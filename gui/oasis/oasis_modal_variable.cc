#include "oasis_modal_variable.h"

using namespace oasis;

void ModalVariable::reset()
{
    m_xymode = XYAbsolute;
    m_placement_x = 0;
    m_placement_y = 0;
    m_text_x = 0;
    m_text_y = 0;
    m_geometry_x = 0;
    m_geometry_y = 0;

    m_defined = 0;
    define_bit(XYModeBit);
    define_bit(PlacementXBit);
    define_bit(PlacementYBit);
    define_bit(TextXBit);
    define_bit(TextYBit);
    define_bit(GeometryXBit);
    define_bit(GeometryYBit);
}

