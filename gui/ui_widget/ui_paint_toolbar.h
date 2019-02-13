#ifndef UI_PAINT_TOOLBAR_H
#define UI_PAINT_TOOLBAR_H

#include <QWidget>
#include <QToolButton>
#include "deftools/global.h"
#include "deftools/defcontrols.h"
namespace ui {

class PaintToolbar : public QWidget
{
    Q_OBJECT


public:
    explicit PaintToolbar(QWidget *parent = 0);

    void init_button();

    void init_connection();

    void updata_toolbar();

    void show_measure();

    void show_normal();

    void show_mark();
    
signals:
    void signal_setPaintStyle(Global::PaintTool);
    void signal_setSnapFlag(Global::SnapFLag);
    void signal_all_clear();
    void siganl_measure_line_clear();
    void signal_mark_clear();
    void signal_measure_table_click();
    
public slots:
    void slot_set_paint_tool(Global::PaintStyle);
    void slot_normal_click(bool);
    void slot_mark_point_click(bool);
    void slot_mark_clear_click();
    void slot_measure_line_click(bool);
    void slot_measure_angle_click(bool);
    void slot_snap_click(bool);
    void slot_eraser_click(bool);
    void slot_table_click();
    void slot_clear_click();
    void slot_measure_line_clear();

private:
    Global::PaintStyle m_paint_style;
    PushButton *m_normal_button;
    PushButton *m_mark_point_button;
    PushButton *m_mark_clear_button;
    PushButton *m_measure_line_button;
    PushButton *m_measure_angle_button;
    PushButton *m_snap_button;
    PushButton *m_eraser_button;
    PushButton *m_clear_button;
    PushButton *m_table_button;
    PushButton *m_measure_clear_button;

    QFrame *m_line_a;
    QFrame *m_line_b;
};
}
#endif // UI_PAINT_TOOLBAR_H
