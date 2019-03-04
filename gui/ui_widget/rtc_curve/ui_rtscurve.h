#ifndef UI_RTSCURVE_H
#define UI_RTSCURVE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QColor>
#include <QPalette>
#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QIcon>
#include <QLabel>

#include "ui_curve_widget.h"

namespace ui{
class RtsCurve : public QWidget
{
    Q_OBJECT
public:
    explicit RtsCurve(QWidget *parent = 0);

    void init_ui();

    void init_toolbar();

    void init_statusbar();
    
signals:
    
public slots:
    void slot_left_action();
    void slot_right_action();
    void slot_home_action();
    void slot_find_action();

private:
    QToolBar *m_toolbar_widget;
    CurveWidget *m_curve_widget;
    QStatusBar *m_statusbar_widget;
    QLabel *m_coordinate_label;
    QLabel *m_cd_value_label;
    
};
}

#endif // UI_RTSCURVE_H
