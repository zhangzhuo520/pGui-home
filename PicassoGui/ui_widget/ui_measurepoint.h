#ifndef UI_MEASUREPOINT_H
#define UI_MEASUREPOINT_H

#include <QObject>
#include <QPoint>
#include <QList>

class LineData{
public:
    QPointF m_first_point;
    QPointF m_last_point;
    double m_distance;
};

class MeasureLine
{
public:
    explicit MeasureLine();
    
    inline void appendLineData(const LineData &linedata)
    {
        m_linedata_list.append(linedata);
    }

    inline QList <LineData> get_point_list()
    {
        return m_linedata_list;
    }

    inline void clear_all_data()
    {
        m_linedata_list.clear();
    }
private:
    QList <LineData> m_linedata_list;
};

#endif // UI_MEASUREPOINT_H
