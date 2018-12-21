#include "ui_measurepoint.h"

namespace ui
{
MeasureLine::MeasureLine()
{
}

double MeasureLine::range = sin(5.0 / 180 * PI);

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

    if(fabs(angle) < range)
    {
        return true;
    }
    else
    {
        return false;
    }

}

void MeasureLine::removeLineData(QPointF p)
{
    int result = -1;
    for(int i = 0 ; i < m_linedata_list.size(); i++)
    {
        if( point_at_edge(p, m_linedata_list[i]))
        {
             m_linedata_list.removeAt(result);
             break;
        }
    }
}

}



