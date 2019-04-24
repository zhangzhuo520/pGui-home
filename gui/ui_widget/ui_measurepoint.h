#ifndef UI_MEASUREPOINT_H
#define UI_MEASUREPOINT_H
#define PI 3.1415927

#include <QObject>
#include <QPoint>
#include <QLineF>
#include <QPointF>
#include <QList>
#include <QColor>
#include <QDebug>
#include <math.h>
#include <utility>

namespace ui
{

struct cmp
{
    bool operator()(std::pair<double,QLineF>& pair1, std::pair<double, QLineF>& pair2)
    {
        return pair1.first > pair2.first;
    }
};

class LineData
{
public:

    LineData():m_line_width(1),m_line_color(Qt::black) { }
    LineData(QPointF p1, QPointF p2, QString distance):m_line_width(1),
                                                      m_line_color(Qt::black),
                                                      m_first_point(p1),
                                                      m_last_point(p2),
                                                      m_distance(distance) {}
    virtual ~LineData() {}
    LineData& operator=(const LineData&);
    bool operator!=(const LineData&);
    bool operator==(const LineData&);

    inline void set_line_width(const int width)
    {
        m_line_width = width;
    }

    inline void set_line_color(const QColor& color)
    {
        m_line_color = color;
    }

    virtual bool point_at_edge(QPointF);

    int m_line_width;
    QColor m_line_color;
    QPointF m_first_point;
    QPointF m_last_point;
    QString m_distance;

};


class ObliqueLineData: public LineData
{
public:
    ObliqueLineData(QLineF, QLineF);

    virtual ~ObliqueLineData() { }
    virtual bool point_at_edge(QPointF);

    QLineF get_first_line() const
    {
        return m_first_line;
    }

    QLineF get_last_line() const
    {
        return m_last_line;
    }
private:
    void calculate_shortest_line();
    void calculate_projection();
    void calculate_intersect_point();
    QLineF m_first_line;
    QLineF m_last_line;
    QLineF m_shortest_line;
};

//qRegisterMetaType("LineData");

class MeasureLine
{
public:
    explicit MeasureLine();

    inline void appendLineData(LineData* linedata)
    {
        m_linedata_list.append(linedata);
    }

    void set_point_list(const QList <LineData*> &);
    inline const QList <LineData*> & get_point_list()
    {
        return m_linedata_list;
    }

    void clear_all_data();

    bool removeLineData(QPointF p);

    LineData* get_select_linedata(const QPointF& p);

    int get_select_lineindex(const QPointF& p);

    static double h_range;
    static double x_y_rang;

private:
    bool point_at_edge(QPointF, LineData*);
    QList <LineData*> m_linedata_list;
};

}

#endif // UI_MEASUREPOINT_H
