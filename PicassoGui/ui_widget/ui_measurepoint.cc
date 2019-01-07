#include "ui_measurepoint.h"

namespace ui
{

double MeasureLine::h_range = sin(5.0 / 180 * PI);
double MeasureLine::x_y_rang = 5;
MeasureLine::MeasureLine()
{
}

void MeasureLine::set_point_list(const QList<LineData> & line_list)
{
    m_linedata_list = line_list;
}

bool MeasureLine::point_at_edge(QPointF p, LineData line)
{    
    double a_x = line.m_first_point.x();
    double a_y = line.m_first_point.y();

    double b_x = p.x();
    double b_y = p.y();

    double c_x = line.m_last_point.x();
    double c_y = line.m_last_point.y();

    double vec_ab_x = b_x - a_x;

    double vec_ab_y = b_y - a_y;
    double length_ab = sqrt(vec_ab_x * vec_ab_x + vec_ab_y * vec_ab_y);

    double vec_ac_x = c_x - a_x;
    double vec_ac_y = c_y - a_y;
    double length_ac = sqrt(vec_ac_x * vec_ac_x + vec_ac_y *vec_ac_y);

    double product = vec_ab_x * vec_ac_y - vec_ab_y * vec_ac_x;
    double angle = product / length_ab / length_ac;

    double x_max = max(c_x,a_x);
    double x_min = min(c_x, a_x);
    double y_max = max(c_y, a_y);
    double y_min = min(a_y, c_y);

    if(((b_x -  x_max) > 0.5) ||
            (( b_x - x_min) < -0.5) ||
                 (( b_y - y_max) > 0.5) ||
                          ((b_y - y_min) < -0.5))
    {
          return false;
    }


    if(fabs(angle) < h_range)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool MeasureLine::removeLineData(QPointF p)
{
    for(int i = 0 ; i < m_linedata_list.size(); i++)
    {
        if( point_at_edge(p, m_linedata_list[i]))
        {
             m_linedata_list.removeAt(i);
             return true;
        }
    }
    return false;
}

LineData::LineData(QPointF p_start, QPointF p_end, double distance):
    m_first_point(p_start),
    m_last_point(p_end),
    m_distance(distance),
    m_line_width(1),
    m_line_color(Qt::black)
{
}

LineData& LineData::operator=(const LineData &lineData)
{
    if (this != &lineData)
    {
        m_distance = lineData.m_distance;
        m_first_point = lineData.m_first_point;
        m_last_point = lineData.m_last_point;
    }
    return *this;
}

bool LineData::operator!=(const LineData &lineData)
{
    if (m_distance != lineData.m_distance &&
        m_first_point != lineData.m_first_point &&
        m_last_point != lineData.m_last_point)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LineData::operator==(const LineData &lineData)
{
    if (m_distance == lineData.m_distance &&
        m_first_point == lineData.m_first_point &&
        m_last_point == lineData.m_last_point)
    {
        return true;
    }
    else
    {
        return false;
    }
}

LineData MeasureLine::get_select_linedata(const QPointF &p)
{
    for(int i = 0 ; i < m_linedata_list.size(); i++)
    {
        if( point_at_edge(p, m_linedata_list[i]))
        {
            return m_linedata_list.at(i);
        }
    }
    return LineData(QPointF(0, 0), QPointF(0, 0), 0);
}

int MeasureLine::get_select_lineindex(const QPointF &p)
{
    for(int i = 0 ; i < m_linedata_list.size(); i++)
    {
        if( point_at_edge(p, m_linedata_list[i]))
        {
            return i;
        }
    }
    return -1;
}
}



