#include "ui_rtscurve.h"
namespace ui {
RtsCurve::RtsCurve(QWidget *parent) :
    QWidget(parent)
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
    VLayout->setStretch(0, 1);
    VLayout->setStretch(1, 9);
    VLayout->setStretch(2, 1);

    setLayout(VLayout);
}

void RtsCurve::init_toolbar()
{
    m_toolbar_widget->addAction(QIcon(":/dfjy/images/rts_left.png"), "", this, SLOT(slot_left_action()));
    m_toolbar_widget->addAction(QIcon(":/dfjy/images/rts_right.png"), "", this, SLOT(slot_right_action()));
    m_toolbar_widget->addAction(QIcon(":/dfjy/images/rts_home.png"), "", this, SLOT(slot_home_action()));
    m_toolbar_widget->addAction(QIcon(":/dfjy/images/rts_find.png"), "", this, SLOT(slot_find_action()));

    m_coordinate_label = new QLabel(" ", this);
    QLabel *space_label = new QLabel(" ", this);
    space_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_toolbar_widget->addWidget(space_label);
    m_toolbar_widget->addWidget(m_coordinate_label);
}

void RtsCurve::init_statusbar()
{
    m_cd_value_label = new QLabel(" ", this);
    m_cd_value_label->setFixedHeight(15);
    m_statusbar_widget->addWidget(m_cd_value_label);
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
