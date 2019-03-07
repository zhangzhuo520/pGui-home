#include "ui_rtscurve.h"
namespace ui {
RtsCurve::RtsCurve(QDialog *parent) :
    QDialog(parent)
{
    init_ui();
    init_toolbar();
    init_statusbar();
}

void RtsCurve::init_ui()
{
    QVBoxLayout *VLayout = new QVBoxLayout(this);
    VLayout->setSpacing(0);
    VLayout->setContentsMargins(0, 0, 0, 0);
    m_toolbar_widget = new QToolBar(this);
    m_curve_widget = new CurveWidget(this);
    m_statusbar_widget = new QStatusBar(this);
    VLayout->addWidget(m_toolbar_widget);
    VLayout->addWidget(m_curve_widget);
    VLayout->addWidget(m_statusbar_widget);
    m_statusbar_widget->setFixedHeight(30);

    setLayout(VLayout);
}

void RtsCurve::init_toolbar()
{
    m_toolbar_widget->addAction(QIcon(":/dfjy/images/rts_left.png"), "", this, SLOT(slot_left_action()));
    m_toolbar_widget->addAction(QIcon(":/dfjy/images/rts_right.png"), "", this, SLOT(slot_right_action()));
    m_toolbar_widget->addAction(QIcon(":/dfjy/images/rts_home.png"), "", this, SLOT(slot_home_action()));
    m_toolbar_widget->addAction(QIcon(":/dfjy/images/rts_find.png"), "", this, SLOT(slot_find_action()));

    m_coordinate_label = new QLabel("0.0  0.0      0.0  0.0", this);
    QLabel *space_label = new QLabel(" ", this);
    space_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_toolbar_widget->addWidget(space_label);
    m_toolbar_widget->addWidget(m_coordinate_label);
}

void RtsCurve::init_statusbar()
{
    m_aicd_label = new QLabel("AI_CD(nm) :", this);
    m_statusbar_widget->addWidget(m_aicd_label);
    m_aicd_value = new QLabel("            ", this);
    m_statusbar_widget->addWidget(m_aicd_value);
    m_ricd_label = new QLabel("RI_CD(nm) :", this);
    m_statusbar_widget->addWidget(m_ricd_label);
    m_ricd_value = new QLabel("            ", this);
    m_statusbar_widget->addWidget(m_ricd_value);
    m_eicd_label = new QLabel("EI_CD(nm) :", this);
    m_statusbar_widget->addWidget(m_eicd_label);
    m_eicd_value = new QLabel("            ", this);
    m_statusbar_widget->addWidget(m_eicd_value);
}

void RtsCurve::slot_left_action()
{
}

void RtsCurve::slot_right_action()
{
}

void RtsCurve::slot_home_action()
{
}

void RtsCurve::slot_find_action()
{
}

}
