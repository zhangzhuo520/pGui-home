#include "ui_measurepoint.h"

#include <QVector2D>
#include <QString>
#include <queue>

namespace ui
{
const double esp = 0.0000001;
double MeasureLine::h_range = sin(5.0 / 180 * PI);
double MeasureLine::x_y_rang = 5;

//LineData::LineData(QPointF p_start, QPointF p_end, QString distance):
//    m_first_point(p_start),
//    m_last_point(p_end),
//    m_distance(distance),
//    m_line_width(1),
//    m_line_color(Qt::black)
//{
//}

// the cross with ab and ap
static double cross(QPointF a, QPointF b, QPointF p )
{
    QVector2D ab(b.x() - a.x(), b.y() - a.y());
    QVector2D ap(p.x() - a.x(), p.y() - a.y());
    return ab.x() * ap.y() - ab.y() * ap.x();
}

static double dot(QPointF a, QPointF b, QPointF p)
{
    QVector2D ab(b.x() - a.x(), b.y() - a.y());
    QVector2D ap(p.x() - a.x(), p.y() - a.y());
    return ab.x() * ap.x() + ab.y() * ap.y();
}

static double dis2(QPointF a, QPointF b)
{
    return (a.x() - b.x()) *(a.x() - b.x()) + (a.y() - b.y()) *(a.y() - b.y());
}

static int dir(QPointF a, QPointF b, QPointF p )
{
    if(cross(a, b, p) < 0) return -1;
    else if(cross(a, b, p) > 0) return 1;
    else if(dot(a, b, p) < 0) return -2;
    else if(dot(a, b, p) > 0)
    {
        if(dis2(a, b) <= dis2(a, p)) return 2;
        return 0;
    }
    return 0;
}

static std::pair<double, QLineF> disMin(QPointF a, QPointF b, QPointF p)
{
    double r =((p.x() - a.x()) * (b.x() - a.x()) + (p.y() - a.y()) *(b.y() - a.y()))/ dis2(a, b);
    if(r <= 0)
    {
        return std::make_pair(sqrt(dis2(a, p)), QLineF(a, p));
    }
    else if(r >= 1)
    {
        return std::make_pair(sqrt(dis2(b, p)), QLineF(b, p));
    }
    else
    {
        double ac = r * sqrt(dis2(a,b));
        double delta_x = b.x() - a.x();
        double delta_y = b.y() - a.y();
        QPointF c(a.x() + delta_x * r, a.y() + delta_y * r);
        return std::make_pair(sqrt(dis2(a, p) - ac * ac ), QLineF(p, c));
    }
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

//const double& max(const double& a, const double& b)
//{
//    return a > b ? a : b;
//}

//const double& min(const double& a, const double& b)
//{
//    return a < b ? a : b;
//}


static bool point_at_line(QPointF p1, QPointF p2, QPointF p)
{
    double a_x = p1.x();
    double a_y = p1.y();

    double b_x = p.x();
    double b_y = p.y();

    double c_x = p2.x();
    double c_y = p2.y();

    double vec_ab_x = b_x - a_x;

    double vec_ab_y = b_y - a_y;
    double length_ab = sqrt(vec_ab_x * vec_ab_x + vec_ab_y * vec_ab_y);

    double vec_ac_x = c_x - a_x;
    double vec_ac_y = c_y - a_y;
    double length_ac = sqrt(vec_ac_x * vec_ac_x + vec_ac_y *vec_ac_y);

    double product = vec_ab_x * vec_ac_y - vec_ab_y * vec_ac_x;
    double angle = product / length_ab / length_ac;

    double x_max = std::max(c_x,a_x);
    double x_min = std::min(c_x, a_x);
    double y_max = std::max(c_y, a_y);
    double y_min = std::min(a_y, c_y);

    if(((b_x -  x_max) > 0) ||
            (( b_x - x_min) < 0) ||
                 (( b_y - y_max) > 0) ||
                          ((b_y - y_min) < 0))
    {
        if ((fabs(y_max - y_min) > esp) && fabs(x_max - x_min) > esp)
        {
            return false;
        }
    }

    if (y_max == y_min )
    {
        if(((b_x - x_max) > 0) || ((b_x - x_min ) < 0))
        {
            return false;
        }
    }

    if (x_max == x_min)
    {
        if (((b_y - y_max) > 0) || ((b_y - y_min) < 0))
        return false;
    }


    if(fabs(angle) < MeasureLine::h_range)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LineData::point_at_edge(QPointF p)
{
    return point_at_line(m_first_point, m_last_point, p);
}


ObliqueLineData::ObliqueLineData(QLineF e1, QLineF e2):LineData(),m_first_line(e1), m_last_line(e2)
{
    calculate_shortest_line();
}

void ObliqueLineData::calculate_intersect_point()
{
    QPointF result;
    QLineF::IntersectType type = m_first_line.intersect(m_last_line,&result);
    if(type == QLineF::BoundedIntersection)
    {
        m_shortest_line.setP1(result);
        m_shortest_line.setP2(result);
        m_first_point = m_shortest_line.p1();
        m_last_point = m_shortest_line.p2();
    }
    else
    {
        return ;
    }
    double dx = m_shortest_line.dx();
    double dy = m_shortest_line.dy();
    m_distance = QString::number(0, 'f' ,6) + "(x :" + QString::number(fabs(dx), 'f', 4) + ", y : " + QString::number(fabs(dy), 'f', 4) + ")";

}

void ObliqueLineData::calculate_shortest_line()
{
    if((dir(m_first_line.p1(), m_first_line.p2(), m_last_line.p1()) * dir(m_first_line.p1(), m_first_line.p2(), m_last_line.p2()) <=0 ) &&
       (dir(m_last_line.p1(),  m_last_line.p2(),  m_first_line.p1())* dir(m_last_line.p1(),  m_last_line.p2(), m_first_line.p2())) <= 0)
    {
        calculate_intersect_point();
    }
    else
    {
        calculate_projection();
    }
}


void ObliqueLineData::calculate_projection()
{
    std::priority_queue<std::pair<double,QLineF>, std::vector<std::pair<double,QLineF> >, cmp> pq;
    pq.push(disMin(m_first_line.p1(), m_first_line.p2(), m_last_line.p1()));
    pq.push(disMin(m_first_line.p1(), m_first_line.p2(), m_last_line.p2()));
    pq.push(disMin(m_last_line.p1(), m_last_line.p2(), m_first_line.p1()));
    pq.push(disMin(m_last_line.p1(), m_last_line.p2(), m_first_line.p2()));
    auto result = pq.top();
    m_shortest_line = result.second;
    m_first_point = m_shortest_line.p1();
    m_last_point = m_shortest_line.p2();
    double dx = m_shortest_line.dx();
    double dy = m_shortest_line.dy();
    m_distance = QString::number(result.first, 'f' ,6) + "(x :" + QString::number(fabs(dx), 'f', 4) + ", y : " + QString::number(fabs(dy), 'f', 4) + ")";

}

bool ObliqueLineData::point_at_edge(QPointF p)
{
    return point_at_line(m_first_line.p1(), m_first_line.p2(), p) ||
           point_at_line(m_last_line.p1(), m_last_line.p2(), p) ||
           point_at_line(m_shortest_line.p1(), m_shortest_line.p2(), p);
}

MeasureLine::MeasureLine()
{
}

void MeasureLine::clear_all_data()
{
    for(int i = 0; i < m_linedata_list.count(); i++)
    {
        delete m_linedata_list[i];
    }
    m_linedata_list.clear();

}

void MeasureLine::set_point_list(const QList<LineData*> & line_list)
{
    for(int i = 0; i < m_linedata_list.count(); i++)
    {
        if(!line_list.contains(m_linedata_list.at(i)))
        {
            delete m_linedata_list[i];
        }
    }

    m_linedata_list = line_list;
}

bool MeasureLine::point_at_edge(QPointF p, LineData* line)
{    
    return line->point_at_edge(p);
}

bool MeasureLine::removeLineData(QPointF p)
{
    for(int i = 0 ; i < m_linedata_list.size(); i++)
    {
        if(point_at_edge(p, m_linedata_list.at(i)))
        {
            delete m_linedata_list.at(i);
            m_linedata_list.removeAt(i);
            return true;
        }
    }
    return false;
}

LineData* MeasureLine::get_select_linedata(const QPointF &p)
{
    for(int i = 0 ; i < m_linedata_list.size(); i++)
    {
        if(point_at_edge(p, m_linedata_list[i]))
        {
            return m_linedata_list.at(i);
        }
    }
    return (LineData*) 0;
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



