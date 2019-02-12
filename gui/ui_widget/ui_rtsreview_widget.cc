#include "ui_rtsreview_widget.h"

namespace ui
{
RtsReviewWidget::RtsReviewWidget(QWidget *parent):
    QWidget(parent)
{
    init_ui();
}

RtsReviewWidget::~RtsReviewWidget()
{

}

void RtsReviewWidget::init_ui()
{
    QVBoxLayout *AllVlayout = new QVBoxLayout(this);
    QVBoxLayout *TopVlayout = new QVBoxLayout();
    QHBoxLayout *RtsHLayout = new QHBoxLayout();
    m_rts_history_label = new QLabel("RTS History: ", this);
    m_rts_del_button = new QPushButton("Del", this);
    m_rts_delall_button = new QPushButton("Del All", this);
    m_rts_history_table = new QTableWidget(this);
    RtsHLayout->addWidget(m_rts_history_label);
    RtsHLayout->addWidget(new QLabel(""));
    RtsHLayout->addWidget(m_rts_del_button);
    RtsHLayout->addWidget(m_rts_delall_button);
    RtsHLayout->setStretch(0, 2);
    RtsHLayout->setStretch(1, 7);
    RtsHLayout->setStretch(2, 1);
    RtsHLayout->setStretch(3, 1);
    TopVlayout->addLayout(RtsHLayout);
    TopVlayout->addWidget(m_rts_history_table);
    AllVlayout->addLayout(TopVlayout);

    QVBoxLayout *MiddleVlayout = new QVBoxLayout();
    QHBoxLayout *Middle_Top_Hlayout = new QHBoxLayout();
    m_rts_setup_label = new QLabel("RTS Setup", this);
    m_setup_combox = new QComboBox(this);
    Middle_Top_Hlayout->addWidget(m_rts_setup_label);
    Middle_Top_Hlayout->addWidget(m_setup_combox);
    Middle_Top_Hlayout->setStretch(0, 1);
    Middle_Top_Hlayout->setStretch(1, 4);
    MiddleVlayout->addLayout(Middle_Top_Hlayout);

    QHBoxLayout *Middle_Middle_Hlayout = new QHBoxLayout();
    m_display_image_label = new QLabel("Display image: ", this);
    m_display_image_combox = new QComboBox(this);
    Middle_Middle_Hlayout->addWidget(m_display_image_label);
    Middle_Middle_Hlayout->addWidget(m_display_image_combox);
    Middle_Middle_Hlayout->setStretch(0, 1);
    Middle_Middle_Hlayout->setStretch(1, 4);
    MiddleVlayout->addLayout(Middle_Middle_Hlayout);

    QHBoxLayout *Middle_bottom_Hlayout = new QHBoxLayout();
    QVBoxLayout *Middle_left_Vlayout = new QVBoxLayout();
    QVBoxLayout *Middle_right_Vlayout = new QVBoxLayout();
    m_display_contour_label = new QLabel("Display contour: ", this);
    QLabel *m_middle_left_label = new QLabel(this);
    Middle_left_Vlayout->addWidget(m_display_contour_label);
    Middle_left_Vlayout->addWidget(m_middle_left_label);
    Middle_left_Vlayout->setStretch(0, 1);
    Middle_left_Vlayout->setStretch(1, 2);

    QHBoxLayout *MiddleAiLayout = new QHBoxLayout();
    m_ai_contour_checkbox = new QCheckBox("AI contour", this);
    m_ai_color_combox = new QComboBox(this);
    m_ai_color_label = new QLabel(this);
    MiddleAiLayout->addWidget(m_ai_contour_checkbox);
    MiddleAiLayout->addWidget(m_ai_color_combox);
    MiddleAiLayout->addWidget(m_ai_color_label);
    MiddleAiLayout->setStretch(0, 2);
    MiddleAiLayout->setStretch(1, 1);
    MiddleAiLayout->setStretch(2, 1);
    Middle_right_Vlayout->addLayout(MiddleAiLayout);
    QHBoxLayout *MiddleRiLayout = new QHBoxLayout();
    m_ri_contour_checkbox = new QCheckBox("RI contour", this);
    m_ri_color_combox = new QComboBox(this);
    m_ri_color_label = new QLabel(this);
    MiddleRiLayout->addWidget(m_ri_contour_checkbox);
    MiddleRiLayout->addWidget(m_ri_color_combox);
    MiddleRiLayout->addWidget(m_ri_color_label);
    MiddleRiLayout->setStretch(0, 2);
    MiddleRiLayout->setStretch(1, 1);
    MiddleRiLayout->setStretch(2, 1);
    Middle_right_Vlayout->addLayout(MiddleRiLayout);
    QHBoxLayout *MiddleEiLayout = new QHBoxLayout();
    m_ei_contour_checkbox = new QCheckBox("EI contour", this);
    m_ei_color_combox = new QComboBox(this);
    m_ei_color_label = new QLabel(this);
    MiddleEiLayout->addWidget(m_ei_contour_checkbox);
    MiddleEiLayout->addWidget(m_ei_color_combox);
    MiddleEiLayout->addWidget(m_ei_color_label);
    MiddleEiLayout->setStretch(0, 2);
    MiddleEiLayout->setStretch(1, 1);
    MiddleEiLayout->setStretch(2, 1);
    Middle_right_Vlayout->addLayout(MiddleEiLayout);

    Middle_bottom_Hlayout->addLayout(Middle_left_Vlayout);
    Middle_bottom_Hlayout->addLayout(Middle_right_Vlayout);
    MiddleVlayout->addLayout(Middle_bottom_Hlayout);
    AllVlayout->addLayout(MiddleVlayout);
    QVBoxLayout *Bottom_Vlayout = new QVBoxLayout();
    QHBoxLayout *Bottom_first_layout = new QHBoxLayout();
    m_cutline_mode_label = new QLabel("Cutline mode: ", this);
    m_cutline_mode_combox = new QComboBox(this);
    m_cutline_mode_button = new QPushButton("Draw", this);
    Bottom_first_layout->addWidget(m_cutline_mode_label);
    Bottom_first_layout->addWidget(m_cutline_mode_combox);
    Bottom_first_layout->addWidget(m_cutline_mode_button);
    Bottom_first_layout->setStretch(0, 3);
    Bottom_first_layout->setStretch(1, 6);
    Bottom_first_layout->setStretch(2, 1);
    Bottom_Vlayout->addLayout(Bottom_first_layout);
    QHBoxLayout *Bottom_second_layout = new QHBoxLayout();
    m_cutline_coord_lable = new QLabel("Cutline coordinate(um): ", this);
    m_start_label = new QLabel("start: ", this);
    m_start_edit = new QLineEdit(this);
    m_end_label = new QLabel("end: ", this);
    m_end_edit = new QLineEdit(this);
    Bottom_second_layout->addWidget(m_cutline_coord_lable);
    Bottom_second_layout->addWidget(m_start_label);
    Bottom_second_layout->addWidget(m_start_edit);
    Bottom_second_layout->addWidget(m_end_label);
    Bottom_second_layout->addWidget(m_end_edit);
    Bottom_second_layout->setStretch(0, 3);
    Bottom_second_layout->setStretch(1, 1);
    Bottom_second_layout->setStretch(2, 2);
    Bottom_second_layout->setStretch(3, 1);
    Bottom_second_layout->setStretch(4, 2);
    Bottom_Vlayout->addLayout(Bottom_second_layout);
    QHBoxLayout *Bottom_three_Hlayout = new QHBoxLayout();
    m_cutline_angle_label = new QLabel("Cutline angle(degree): ", this);
    m_cutline_angle_edit = new QLineEdit(this);
    Bottom_three_Hlayout->addWidget(m_cutline_angle_label);
    Bottom_three_Hlayout->addWidget(m_cutline_angle_edit);
    Bottom_three_Hlayout->setStretch(0, 4);
    Bottom_three_Hlayout->setStretch(1, 2);
    Bottom_Vlayout->addLayout(Bottom_three_Hlayout);
    QHBoxLayout *Bottom_four_Hlayout = new QHBoxLayout();
    m_cutline_history_label = new QLabel("Cutline History: ", this);
    m_cutline_del_button = new QPushButton("Del", this);
    m_cutline_delall_button = new QPushButton("Del All", this);
    Bottom_four_Hlayout->addWidget(m_cutline_history_label);
    Bottom_four_Hlayout->addWidget(new QLabel(" "));
    Bottom_four_Hlayout->addWidget(m_cutline_del_button);
    Bottom_four_Hlayout->addWidget(m_cutline_delall_button);
    Bottom_four_Hlayout->setStretch(0, 2);
    Bottom_four_Hlayout->setStretch(1, 4);
    Bottom_four_Hlayout->setStretch(2, 1);
    Bottom_four_Hlayout->setStretch(3, 1);
    Bottom_Vlayout->addLayout(Bottom_four_Hlayout);
    m_cutline_table = new QTableWidget(this);
    Bottom_Vlayout->addWidget(m_cutline_table);
    AllVlayout->addLayout(Bottom_Vlayout);
    setLayout(AllVlayout);
}

}
