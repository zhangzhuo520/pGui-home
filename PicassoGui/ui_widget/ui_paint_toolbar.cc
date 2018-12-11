#include "ui_paint_toolbar.h"
namespace ui{
PaintToolbar::PaintToolbar(QWidget *parent) :
    QWidget(parent)
{
    setFixedHeight(20);
    init_button();
    init_connection();
}

void PaintToolbar::init_button()
{
    m_normal_button = new PushButton(this);
    m_normal_button->setGeometry(2, 2, 18, 18);
    m_normal_button->setCheckable(true);
    m_normal_button->setIcon(QIcon(":/dfjy/images/tool.png"));
    m_mark_point_button = new PushButton(this);
    m_mark_point_button->setCheckable(true);
    m_mark_point_button->setGeometry(20, 2, 18, 18);
    m_mark_point_button->setIcon(QIcon(":/dfjy/images/cross.png"));
    m_measure_line_button = new PushButton(this);
    m_measure_line_button->setCheckable(true);
    m_measure_line_button->setGeometry(20, 2, 18, 18);
    m_measure_line_button->setIcon(QIcon(":/dfjy/images/measure_line.png"));
    m_measure_angle_button = new PushButton(this);
    m_measure_angle_button->setCheckable(true);
    m_measure_angle_button->setGeometry(40, 2, 18, 18);
    m_measure_angle_button->setIcon(QIcon(":/dfjy/images/45du.png"));
    m_mark_point_button->hide();
    m_measure_line_button->hide();
    m_measure_angle_button->hide();
}

void PaintToolbar::init_connection()
{
    connect(m_normal_button, SIGNAL(clicked(bool)), this, SLOT(slot_normal_click(bool)));
    connect(m_mark_point_button, SIGNAL(clicked(bool)), this, SLOT(slot_mark_point_click(bool)));
    connect(m_measure_line_button, SIGNAL(clicked(bool)), this, SLOT(slot_measure_line_click(bool)));
    connect(m_measure_angle_button, SIGNAL(clicked(bool)), this, SLOT(slot_measure_angle_click(bool)));
}


void PaintToolbar::slot_setPaintStyle(ui::Global::PaintStyle paint_style)
{
    m_paint_style = paint_style;
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
        emit signal_setPaintStyle(Global::MarkCross);
    }

}

void PaintToolbar::slot_measure_line_click(bool flag)
{
    if (flag)
    {
        m_measure_line_button->setChecked(true);
        m_measure_angle_button->setChecked(false);
        emit signal_setPaintStyle(Global::MeasureLine);
    }
    else
    {
        m_measure_line_button->setChecked(false);
    }
}

void PaintToolbar::slot_measure_angle_click(bool flag)
{
    if (flag)
    {
        m_measure_line_button->setChecked(false);
        m_measure_angle_button->setChecked(true);
        emit signal_setPaintStyle(Global::MeasureAngle);
    }
    else
    {
        m_measure_angle_button->setChecked(false);
    }
}


void PaintToolbar::updata_toolbar()
{
    if(m_paint_style == Global::Normal)
    {
    }
    else if(m_paint_style == Global::Mark)
    {
        m_mark_point_button->show();
        m_measure_line_button->hide();
        m_measure_angle_button->hide();
    }
    else if(m_paint_style == Global::Measrue)
    {
        m_measure_line_button->show();
        m_measure_angle_button->show();
        m_mark_point_button->hide();
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
