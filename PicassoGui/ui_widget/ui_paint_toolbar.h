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
    
public slots:
    void slot_setPaintStyle(Global::PaintStyle);

    void slot_normal_click(bool);

    void slot_mark_point_click(bool);

    void slot_measure_line_click(bool);

    void slot_measure_angle_click(bool);

private:
    Global::PaintStyle m_paint_style;
    PushButton *m_normal_button;
    PushButton *m_mark_point_button;
    PushButton *m_measure_line_button;
    PushButton *m_measure_angle_button;
};
}
#endif // UI_PAINT_TOOLBAR_H
