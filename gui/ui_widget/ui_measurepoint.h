#ifndef UI_MEASUREPOINT_H
#define UI_MEASUREPOINT_H
#define PI 3.1415927

#include <QObject>
#include <QPoint>
#include <QList>
#include <QColor>
#include <QDebug>
#include <math.h>
namespace ui
{

class LineData{
public:
    LineData(QPointF, QPointF, double);
    ~LineData(){}

    LineData& operator=(const LineData&);
    bool operator!=(const LineData&);
    bool operator==(const LineData&);

    inline void set_line_width(const int &width)
    {
         m_line_width = width;
    }

    inline void set_line_color(const QColor &color)
    {
         m_line_color = color;
    }


    QPointF m_first_point;
    QPointF m_last_point;
    double m_distance;

    int m_line_width;
    QColor m_line_color;
};

//qRegisterMetaType("LineData");

class MeasureLine
{
public:
    explicit MeasureLine();

    inline void appendLineData(const LineData &linedata)
    {
        m_linedata_list.append(linedata);
    }

    void set_point_list(const QList <LineData> &);
    inline QList <LineData> & get_point_list()
    {
        return m_linedata_list;
    }

    inline void clear_all_data()
    {
        m_linedata_list.clear();
    }

    bool removeLineData(QPointF p);

    LineData get_select_linedata(const QPointF& p);

    int get_select_lineindex(const QPointF& p);

    static double h_range;
    static double x_y_rang;

private:
    inline const double& max(const double& a, const double& b)
    {
        return a > b ? a : b;
    }

    inline const double& min(const double& a, const double& b)
    {
        return a < b ? a : b;
    }


    bool point_at_edge(QPointF, LineData);
    QList <LineData> m_linedata_list;
};

}

#endif // UI_MEASUREPOINT_H
