#ifndef UI_MEASUREPOINT_H
#define UI_MEASUREPOINT_H

#include <QObject>
#include <QPoint>

class MeasurePoint :public QObject
{
    Q_OBJECT
public:
    explicit MeasurePoint(QObject *parent = 0);
    

private:
    QPoint m_first_point;
    QPoint m_end_point;
};

#endif // UI_MEASUREPOINT_H
