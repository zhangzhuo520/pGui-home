#ifndef UI_MEASUREPOINT_H
#define UI_MEASUREPOINT_H

#include <QObject>
#include <QPoint>
#include <QList>

class LineData{
public:
    QPoint m_first_point;
    QPoint m_last_point;
};

class MeasureLine
{
public:
    explicit MeasureLine();
    
    inline void appendLineData(const LineData &linedata)
    {
        m_linedata_list.append(linedata);
    }

    QList <LineData> get_point_list();

    void clear_all_data();
private:
    QList <LineData> m_linedata_list;
};

#endif // UI_MEASUREPOINT_H
