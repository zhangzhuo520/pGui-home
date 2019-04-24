#include "ui_paint_toolbar.h"
namespace ui{
PaintToolbar::PaintToolbar(QWidget *parent) :
    QWidget(parent),
    is_first_ruler(true)
{
    setFixedHeight(20);
    init_button();
    init_connection();
}

void PaintToolbar::init_button()
{
    m_mark_point_button = new PushButton(this);
    m_mark_point_button->setObjectName("markButton");
    m_mark_point_button->setToolTip("Mark");
    m_mark_point_button->setCheckable(true);
    m_mark_point_button->setGeometry(2, 2, 18, 18);
    m_mark_point_button->setIcon(QIcon(":/dfjy/images/cross.png"));

    m_mark_clear_button = new PushButton(this);
    m_mark_clear_button->setObjectName("mark_clear_button");
    m_mark_clear_button->setToolTip("delete all");
    m_mark_clear_button->setIcon(QIcon(":/dfjy/images/clean.png"));
    m_mark_clear_button->setGeometry(30, 2, 18, 18);


    m_snap_button = new PushButton(this);
    m_snap_button->setObjectName("snapButton");
    m_snap_button->setIcon(QIcon(":/dfjy/images/snap.png"));
    m_snap_button->setToolTip("Snap");
    m_snap_button->setCheckable(true);
    m_snap_button->setGeometry(2, 2, 18, 18);

    m_clear_button = new PushButton(this);
    m_clear_button->setObjectName("clear_button");
    m_clear_button->setToolTip("delete all");
    m_clear_button->setIcon(QIcon(":/dfjy/images/clean.png"));
    m_clear_button->setGeometry(2, 2, 18, 18);

    m_line_a = new QFrame(this);
    m_line_a->setFrameShape(QFrame::VLine);
    m_line_a->setFrameShadow(QFrame::Sunken);
    m_line_a->setGeometry(24, 2, 2, 18);

    m_measure_line_button = new PushButton(this);
    m_measure_line_button->setObjectName("measureLineButton");
    m_measure_line_button->setIcon(QIcon(":/dfjy/images/measure_line.png"));
    m_measure_line_button->setToolTip("Measure Line");
    m_measure_line_button->setCheckable(true);
    m_measure_line_button->setGeometry(30, 2, 18, 18);

    m_measure_angle_button = new PushButton(this);
    m_measure_angle_button->setObjectName("measureAngleButton");
    m_measure_angle_button->setToolTip("Angular Measure Line");
    m_measure_angle_button->setIcon(QIcon(":/dfjy/images/45du.png"));
    m_measure_angle_button->setCheckable(true);
    m_measure_angle_button->setGeometry(50, 2, 18, 18);

    m_line_b = new QFrame(this);
    m_line_b->setFrameShape(QFrame::VLine);
    m_line_b->setFrameShadow(QFrame::Sunken);
    m_line_b->setGeometry(72, 2, 2, 18);

    m_measure_oblique_button = new PushButton(this);
    m_measure_oblique_button->setObjectName("measureObliqueButton");
    m_measure_oblique_button->setToolTip("Oblique Measure Line");
    m_measure_oblique_button->setIcon(QIcon(":/dfjy/images/oblique_measure.png"));
    m_measure_oblique_button->setCheckable(true);
    m_measure_oblique_button->setGeometry(78, 2, 18, 18);

    m_line_c = new QFrame(this);
    m_line_c->setFrameShape(QFrame::VLine);
    m_line_c->setFrameShadow(QFrame::Sunken);
    m_line_c->setGeometry(98, 2, 2, 18);

    m_eraser_button = new PushButton(this);
    m_eraser_button->setObjectName("measureEraserButton");
    m_eraser_button->setToolTip("Delete");
    m_eraser_button->setIcon(QIcon(":/dfjy/images/eraser.png"));
    m_eraser_button->setCheckable(true);
    m_eraser_button->setGeometry(104, 2, 18, 18);

    m_measure_clear_button = new PushButton(this);
    m_measure_clear_button->setObjectName("measureClearButton");
    m_measure_clear_button->setToolTip("Delete");
    m_measure_clear_button->setIcon(QIcon(":/dfjy/images/clean.png"));
    m_measure_clear_button->setGeometry(134, 2, 18, 18);

    m_table_button = new PushButton(this);
    m_table_button->setObjectName("tableButton");
    m_table_button->setToolTip("Table");
    m_table_button->setIcon(QIcon(":/dfjy/images/measure_table.png"));
    m_table_button->setCheckable(true);
    m_table_button->setGeometry(164, 2, 18, 18);

    m_snap_button->hide();
    m_mark_point_button->hide();
    m_mark_clear_button->hide();
    m_measure_line_button->hide();
    m_measure_angle_button->hide();
    m_measure_oblique_button->hide();

    m_clear_button->show();
    m_eraser_button->hide();
    m_table_button->hide();
    m_measure_clear_button->hide();
    m_line_a->hide();
    m_line_b->hide();
    m_line_c->hide();
}

void PaintToolbar::init_connection()
{
    connect(m_mark_point_button, SIGNAL(clicked(bool)), this, SLOT(slot_mark_point_click(bool)));
    connect(m_mark_clear_button, SIGNAL(clicked()), this, SLOT(slot_mark_clear_click()));
    connect(m_measure_line_button, SIGNAL(clicked(bool)), this, SLOT(slot_measure_line_click(bool)));
    connect(m_measure_angle_button, SIGNAL(clicked(bool)), this, SLOT(slot_measure_angle_click(bool)));
    connect(m_measure_oblique_button, SIGNAL(clicked(bool)), this, SLOT(slot_measure_oblique_click(bool)));
    connect(m_snap_button, SIGNAL(clicked(bool)), this, SLOT(slot_snap_click(bool)));
    connect(m_clear_button, SIGNAL(clicked()), this, SLOT(slot_clear_click()));
    connect(m_eraser_button, SIGNAL(clicked(bool)), this, SLOT(slot_eraser_click(bool)));
    connect(m_table_button, SIGNAL(clicked()), this, SLOT(slot_table_click()));
    connect(m_measure_clear_button, SIGNAL(clicked()), this, SLOT(slot_measure_line_clear()));
}

void PaintToolbar::slot_set_paint_tool(ui::Global::PaintStyle paint_style)
{
    m_paint_style = paint_style;
    if (is_first_ruler)
    {
        m_measure_line_button->setChecked(true);
        is_first_ruler = false;
    }
    updata_toolbar();
}

void PaintToolbar::slot_normal_click(bool flag)
{
    if (flag)
    {
        emit signal_setPaintStyle(Global::Nothing);
    }
}

void PaintToolbar::slot_mark_point_click(bool flag)
{
    if (flag)
    {
        m_mark_point_button->setChecked(true);
        m_measure_line_button->setChecked(false);
        m_eraser_button->setChecked(false);
        m_measure_angle_button->setChecked(false);
        m_measure_oblique_button->setChecked(false);
        emit signal_setPaintStyle(Global::MarkCross);
    }
    else
    {
        emit signal_setPaintStyle(Global::Nothing);
    }

}

void PaintToolbar::slot_mark_clear_click()
{
    emit signal_mark_clear();
}

void PaintToolbar::slot_measure_line_click(bool flag)
{
    if (flag)
    {
        m_measure_angle_button->setChecked(false);
        m_measure_oblique_button->setChecked(false);
        m_eraser_button->setChecked(false);
        m_mark_point_button->setChecked(false);
        emit signal_setPaintStyle(Global::MeasureLine);
    }
    else
    {
        emit signal_setPaintStyle(Global::Nothing);
    }
}

void PaintToolbar::slot_measure_angle_click(bool flag)
{
    if (flag)
    {
        m_measure_line_button->setChecked(false);
        m_measure_oblique_button->setChecked(false);
        m_eraser_button->setChecked(false);
        m_mark_point_button->setChecked(false);
        emit signal_setPaintStyle(Global::MeasureAngle);
    }
    else
    {
        emit signal_setPaintStyle(Global::Nothing);
    }
}

void PaintToolbar::slot_measure_oblique_click(bool flag)
{
    if(flag)
    {
        m_measure_line_button->setChecked(false);
        m_measure_angle_button->setChecked(false);
        m_eraser_button->setChecked(false);
        m_mark_point_button->setChecked(false);
        emit signal_setPaintStyle(Global::MeasureOblique);
    }
    else
    {
        emit signal_setPaintStyle(Global::Nothing);
    }
}

void PaintToolbar::slot_snap_click(bool flag)
{
    if (flag)
    {
        m_eraser_button->setChecked(false);
        emit signal_setSnapFlag(Global::SnapOpen);
    }
    else
    {
        emit signal_setSnapFlag(Global::SnapClose);
    }
}

void PaintToolbar::slot_eraser_click(bool flag)
{
    if(flag)
    {
        m_measure_line_button->setChecked(false);
        m_measure_angle_button->setChecked(false);
        m_measure_oblique_button->setChecked(false);
        m_mark_point_button->setChecked(false);
        emit signal_setPaintStyle(Global::RemoveLine);
    }
    else
    {
        emit signal_setPaintStyle(Global::Nothing);
    }
}

void PaintToolbar::slot_table_click()
{
    emit signal_measure_table_click();
}

void PaintToolbar::slot_clear_click()
{
    emit signal_all_clear();
}

void PaintToolbar::slot_measure_line_clear()
{
    emit signal_measure_line_clear();
}

void PaintToolbar::updata_toolbar()
{
    if(m_paint_style == Global::Normal)
    {
        m_clear_button->show();
        m_mark_point_button->hide();
        m_mark_clear_button->hide();
        m_measure_line_button->hide();
        m_measure_angle_button->hide();
        m_measure_oblique_button->hide();
        m_line_a->hide();
        m_line_b->hide();
        m_line_c->hide();
        m_snap_button->hide();
        m_eraser_button->hide();
        m_measure_clear_button->hide();
        m_table_button->hide();
        emit signal_setPaintStyle(Global::Nothing);
    }
    else if(m_paint_style == Global::Mark)
    {
        m_mark_point_button->show();
        m_line_a->show();
        m_mark_clear_button->show();
        m_measure_line_button->hide();
        m_measure_angle_button->hide();
        m_measure_oblique_button->hide();
        m_line_b->hide();
        m_line_c->hide();
        m_clear_button->hide();
        m_snap_button->hide();
        m_eraser_button->hide();
        m_measure_clear_button->hide();
        m_table_button->hide();
        emit signal_setPaintStyle(Global::Nothing);
    }
    else if(m_paint_style == Global::Measrue)
    {
        m_measure_line_button->show();
        m_measure_angle_button->show();
        m_snap_button->show();
        m_eraser_button->show();
        m_measure_clear_button->show();
        m_measure_oblique_button->show();
        m_clear_button->hide();
        m_line_a->show();
        m_line_b->show();
        m_line_c->show();
        m_table_button->show();
        m_mark_point_button->hide();
        m_mark_clear_button->hide();
        if (m_measure_line_button->isChecked())
        {
            emit signal_setPaintStyle(Global::MeasureLine);
        }
        else if (m_eraser_button->isChecked())
        {
             emit signal_setPaintStyle(Global::RemoveLine);
        }
        else if (m_measure_angle_button->isChecked())
        {
             emit signal_setPaintStyle(Global::MeasureAngle);
        }
        else if (m_mark_point_button->isChecked())
        {
             emit signal_setPaintStyle(Global::MarkCross);
        }
        else if (m_measure_oblique_button->isChecked())
        {
             emit signal_setPaintStyle(Global::MeasureOblique);
        }
        else
        {
             emit signal_setPaintStyle(Global::Nothing);
        }

        if(m_snap_button->isChecked())
        {
            emit signal_setSnapFlag(Global::SnapOpen);
        }
        else
        {
           emit signal_setSnapFlag(Global::SnapClose);
        }


    }
}

void PaintToolbar::show_measure()
{
}

void PaintToolbar::show_normal()
{
}

void PaintToolbar::show_mark()
{
}

}
