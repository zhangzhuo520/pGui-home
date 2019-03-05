#include "oasis_types.h"
#include "oasis_exception.h"

using namespace oasis;

int32 OasisDebug::m_level = 0;
bool OasisDebug::m_error_disp = true;
bool OasisComply::m_comply = true;


void oasis::oasis_error(const std::string& expr, const std::string& file, int line)
{
    if (OasisDebug::oasis_error_disp())
    {
       OAS_ERR << "assertion fail:" << expr << ", " << file << ":" << line << OAS_ENDL;
    }
    throw OasisException("oasis assertion failed: " + expr + " in " + file );
}

bool OasisString::is_astring() const
{
    for(uint32 i = 0; i < m_value.size(); ++i)
    {
        if(m_value[i] < AsciiSpace || m_value[i] > AsciiPrintableMax)
        {
            return false;
        }
    }
    return true;
}

bool OasisString::is_nstring() const
{
    if(m_value.empty())
    {
        return false;
    }
    for(uint32 i = 0; i < m_value.size(); ++i)
    {
        if(m_value[i] < AsciiPrintableMin || m_value[i] > AsciiPrintableMax)
        {
            return false;
        }
    }
    return true;
}

void OasisDelta::setValue(const Direction& dir,const uint64& value)
{
    m_dir = dir;
    switch(dir)
    {
        case OasisDelta::East:
            m_dx = value;
            m_dy = 0;
            break;
        case OasisDelta::North:
            m_dx = 0;
            m_dy = value;
            break;
        case OasisDelta::West:
            m_dx = -value;
            m_dy = 0;
            break;
        case OasisDelta::South:
            m_dx = 0;
            m_dy = -value;
            break;
        case OasisDelta::NorthEast:
            m_dx = value;
            m_dy = value;
            break;
        case OasisDelta::NorthWest:
            m_dx = -value;
            m_dy = value;
            break;
        case OasisDelta::SouthWest:
            m_dx = -value;
            m_dy = -value;
            break;
        case OasisDelta::SouthEast:
            m_dx = value;
            m_dy = -value;
            break;
        default:
            //error
            break;
    }
}

void OasisDelta::getValue(Direction& dir, uint64& value) const
{
    dir = m_dir;
    switch(dir)
    {
        case OasisDelta::East:
            value = m_dx;
            break;
        case OasisDelta::North:
            value = m_dy;
            break;
        case OasisDelta::West:
            value = -m_dx;
            break;
        case OasisDelta::South:
            value = -m_dy;
            break;
        case OasisDelta::NorthEast:
            value = m_dx;
            break;
        case OasisDelta::NorthWest:
            value = m_dy;
            break;
        case OasisDelta::SouthWest:
            value = -m_dx;
            break;
        case OasisDelta::SouthEast:
            value = -m_dy;
            break;
        case OasisDelta::Any:
            break;
        default:
            //error
            break;
    }
}

void OasisRepetition::get_disps(std::vector<Point>& disps) const
{
    switch(m_type)
    {
        case ORT_Regular2D:
            disps.reserve((m_dimensionY + 2) * (m_dimensionX + 2));
            for(uint32 j = 0; j < m_dimensionY + 2; ++j)
            {
                for(uint32 i = 0; i < m_dimensionX + 2; ++i)
                {
                    disps.push_back(Point(i * m_spaces[0], j * m_spaces[1]));
                }
            }
            break;
        case ORT_HorizontalRegular:
            disps.reserve(m_dimensionX + 2);
            for(uint32 i = 0; i < m_dimensionX + 2; ++i)
            {
                disps.push_back(Point(i * m_spaces[0], 0));
            }
            break;
        case ORT_VerticalRegular:
            disps.reserve(m_dimensionY + 2);
            for(uint32 j = 0; j < m_dimensionY + 2; ++j)
            {
                disps.push_back(Point(0, j * m_spaces[0]));
            }
            break;
        case ORT_HorizontalIrregular:
        {
            disps.reserve(m_dimensionX + 2);
            disps.push_back(Point());
            int64 space = 0;
            for(uint32 i = 1; i < m_dimensionX + 2; ++i)
            {
                space += m_spaces[i - 1];
                disps.push_back(Point(space, 0));
            }
            break;
        }
        case ORT_HorizontalIrregularGrid:
        {
            disps.reserve(m_dimensionX + 2);
            disps.push_back(Point());
            int64 space = 0;
            for(uint32 i = 1; i < m_dimensionX + 2; ++i)
            {
                space += m_spaces[i - 1] * m_grid;
                disps.push_back(Point(space, 0));
            }
            break;
        }
        case ORT_VerticalIrregular:
        {
            disps.reserve(m_dimensionY + 2);
            disps.push_back(Point());
            int64 space = 0;
            for(uint32 j = 1; j < m_dimensionY + 2; ++j)
            {
                space += m_spaces[j - 1];
                disps.push_back(Point(0, space));
            }
            break;
        }
        case ORT_VerticalIrregularGrid:
        {
            disps.reserve(m_dimensionY + 2);
            disps.push_back(Point());
            int64 space = 0;
            for(uint32 j = 1; j < m_dimensionY + 2; ++j)
            {
                space += m_spaces[j - 1] * m_grid;
                disps.push_back(Point(0, space));
            }
            break;
        }
        case ORT_DiagonalRegular2D:
            disps.reserve((m_dimensionN + 2) * (m_dimensionM + 2));
            for(uint32 i = 0; i < m_dimensionN + 2; ++i)
            {
                for(uint32 j = 0; j < m_dimensionM + 2; ++j)
                {
                    disps.push_back(Point(i * m_deltas[0].m_dx + j * m_deltas[1].m_dx, i * m_deltas[0].m_dy + j * m_deltas[1].m_dy));
                }
            }
            break;
        case ORT_DiagonalRegular1D:
            disps.reserve(m_dimension + 2);
            for(uint32 i = 0; i < m_dimension + 2; ++i)
            {
                disps.push_back(Point(i * m_deltas[0].m_dx, i * m_deltas[0].m_dy));
            }
            break;
        case ORT_Arbitory:
        {
            disps.reserve(m_dimension + 2);
            disps.push_back(Point());
            int64 dx = 0;
            int64 dy = 0;
            for(uint32 i = 1; i < m_dimension + 2; ++i)
            {
                dx += m_deltas[i - 1].m_dx;
                dy += m_deltas[i - 1].m_dy;
                disps.push_back(Point(dx, dy));
            }
            break;
        }
        case ORT_ArbitoryGrid:
        {
            disps.reserve(m_dimension + 2);
            disps.push_back(Point());
            int64 dx = 0;
            int64 dy = 0;
            for(uint32 i = 1; i < m_dimension + 2; ++i)
            {
                dx += m_deltas[i - 1].m_dx * m_grid;
                dy += m_deltas[i - 1].m_dy * m_grid;
                disps.push_back(Point(dx, dy));
            }
            break;
        }
        default:
            oas_assert(false);
    }
}

void OasisRepetition::get_bbox(Box& bbox) const
{
    bbox.set_left(0);
    bbox.set_bottom(0);
    bbox.set_right(0);
    bbox.set_top(0);
    switch(m_type)
    {
        case OasisRepetition::ORT_ReusePrevious:
            oas_assert(false);
            break;
        case OasisRepetition::ORT_Regular2D:
            bbox.set_right((m_dimensionX + 1) * m_spaces[0]);
            bbox.set_top((m_dimensionY + 1) * m_spaces[1]);
            break;
        case OasisRepetition::ORT_HorizontalRegular:
            bbox.set_right((m_dimensionX + 1) * m_spaces[0]);
            break;
        case OasisRepetition::ORT_VerticalRegular:
            bbox.set_top((m_dimensionY + 1) * m_spaces[0]);
            break;
        case OasisRepetition::ORT_HorizontalIrregular:
        {
            int64 space = 0;
            for(uint32 i = 1; i < m_dimensionX + 2; ++i)
            {
                space += m_spaces[i - 1];
            }
            bbox.set_right(space);
            break;
        }
        case OasisRepetition::ORT_HorizontalIrregularGrid:
        {
            int64 space = 0;
            for(uint32 i = 1; i < m_dimensionX + 2; ++i)
            {
                space += m_spaces[i - 1] * m_grid;
            }
            bbox.set_right(space);
            break;
        }
        case OasisRepetition::ORT_VerticalIrregular:
        {
            int64 space = 0;
            for(uint32 j = 1; j < m_dimensionY + 2; ++j)
            {
                space += m_spaces[j - 1];
            }
            bbox.set_top(space);
            break;
        }
        case OasisRepetition::ORT_VerticalIrregularGrid:
        {
            int64 space = 0;
            for(uint32 j = 1; j < m_dimensionY + 2; ++j)
            {
                space += m_spaces[j - 1] * m_grid;
            }
            bbox.set_top(space);
            break;
        }
        case OasisRepetition::ORT_DiagonalRegular2D:
        {
            int64 dxN = (m_dimensionN + 1) * m_deltas[0].m_dx;
            int64 dyN = (m_dimensionN + 1) * m_deltas[0].m_dy;
            int64 dxM = (m_dimensionM + 1) * m_deltas[1].m_dx;
            int64 dyM = (m_dimensionM + 1) * m_deltas[1].m_dy;
            bbox.update(Point(dxN, dyN));
            bbox.update(Point(dxM, dyM));
            bbox.update(Point(dxN + dxM, dyN + dyM));
            break;
        }
        case OasisRepetition::ORT_DiagonalRegular1D:
        {
            int64 dx = (m_dimension + 1) * m_deltas[0].m_dx;
            int64 dy = (m_dimension + 1) * m_deltas[0].m_dy;
            bbox.update(Point(dx, dy));
            break;
        }
        case OasisRepetition::ORT_Arbitory:
        {
            int64 dx = 0;
            int64 dy = 0;
            for(uint32 i = 1; i < m_dimension + 2; ++i)
            {
                dx += m_deltas[i - 1].m_dx;
                dy += m_deltas[i - 1].m_dy;
                bbox.update(Point(dx, dy));
            }
            break;
        }
        case OasisRepetition::ORT_ArbitoryGrid:
        {
            int64 dx = 0;
            int64 dy = 0;
            for(uint32 i = 1; i < m_dimension + 2; ++i)
            {
                dx += m_deltas[i - 1].m_dx * m_grid;
                dy += m_deltas[i - 1].m_dy * m_grid;
                bbox.update(Point(dx, dy));
            }
            break;
        }
        default:
            oas_assert(false);
            break;
    }
}

uint64 OasisRepetition::get_dimension() const
{
    switch(m_type)
    {
        case OasisRepetition::ORT_ReusePrevious:
            oas_assert(false);
            break;
        case OasisRepetition::ORT_Regular2D:
            return (m_dimensionX + 2) * (m_dimensionY + 2);
            break;
        case OasisRepetition::ORT_HorizontalRegular:
        case OasisRepetition::ORT_HorizontalIrregular:
        case OasisRepetition::ORT_HorizontalIrregularGrid:
            return m_dimensionX + 2;
            break;
        case OasisRepetition::ORT_VerticalRegular:
        case OasisRepetition::ORT_VerticalIrregular:
        case OasisRepetition::ORT_VerticalIrregularGrid:
            return m_dimensionY + 2;
            break;
        case OasisRepetition::ORT_DiagonalRegular2D:
            return (m_dimensionN + 2) * (m_dimensionM + 2);
            break;
        case OasisRepetition::ORT_DiagonalRegular1D:
        case OasisRepetition::ORT_Arbitory:
        case OasisRepetition::ORT_ArbitoryGrid:
            return m_dimension + 2;
            break;
        default:
            oas_assert(false);
            break;
    }
    return 0;
}


