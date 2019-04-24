#include "ui_rtsreview_dialog.h"
#include "ui_rtscurve.h"
#include "image_worker.h"
#include "rts_manager.h"
#include "../ui_mainwindow.h"
#include "../deftools/cmessagebox.h"

#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLine>
namespace ui
{
RtsReviewDialog::RtsReviewDialog(QWidget *parent):
    m_mainwindow(static_cast <MainWindow *>(parent)),
    m_current_image_path(""),
    m_rts_count_flag(0),
    m_image_count_flag(0)
{
    init_ui();
    init_connection();
    slot_cutline_mode_change(m_cutline_mode_combox->currentIndex());

    m_imagedata_worker = new ImageDataWorker();
}

RtsReviewDialog::~RtsReviewDialog()
{

}

void RtsReviewDialog::append_history_data(const QString & data)
{
    m_rts_history_table->append_data(data);
}

void RtsReviewDialog::append_setup_combox(const QString & string)
{
    m_setup_combox->addItem(string);
}

void RtsReviewDialog::rts_setup_clear()
{
    m_setup_combox->clear();
    m_display_image_combox->clear();
}

void RtsReviewDialog::set_cutline_table( const QList <LineData*> & line_data_list)
{
    m_cutline_table->set_cutline_list(line_data_list);
    QString image_path = "";
    if (m_rtscurve_widget->isHidden())
    {
        m_rtscurve_widget->draw_curve(image_path, *line_data_list.last());
        m_rtscurve_widget->show();
    }
    else
    {
        m_rtscurve_widget->draw_curve(image_path, *line_data_list.last());
    }
}

void RtsReviewDialog::append_image_commbox(const QString & image)
{
    m_display_image_combox->addItem(image);
}

void RtsReviewDialog::clear_image_commbox()
{
    m_display_image_combox->clear();
}

void RtsReviewDialog::updata_image_setup()
{
    m_rts_manager = m_mainwindow->rts_manager();
    int index = m_setup_combox->currentIndex();
    RtsInfo info = m_rts_manager->rts_info(index);
    m_current_image_list.clear();
    m_current_image_list = info.image_list();
    clear_image_commbox();
    for (int i = 0; i < m_current_image_list.count(); i ++)
    {
        m_display_image_combox->addItem(m_current_image_list.at(i).split("/").last());
    }

    if (m_display_image_combox->currentIndex() == -1)
    {
        return;
    }

    m_current_image_path = m_current_image_list.at(m_display_image_combox->currentIndex());
    m_mainwindow->enable_rts_image(m_current_image_path);
}

void RtsReviewDialog::slot_delete_index()
{
    if (m_rts_history_table->currentIndex().isValid())
    {
        m_rts_history_table->delete_data(m_rts_history_table->currentIndex().row());
    }
    else
    {
        m_rts_history_table->delete_data(m_rts_history_table->count() - 1);
    }
}

void RtsReviewDialog::slot_delete_all_index()
{
    m_rts_history_table->delete_all_data();
}

void RtsReviewDialog::slot_draw_cutline()
{
    parse_image();
    switch (m_cutline_mode_combox->currentIndex())
    {
    case 0:
        emit signal_paint_cutline(Global::HV, QVariant());
        break;
    case 1:
        emit signal_paint_cutline(Global::HVD, QVariant());
        break;
    case 2:
        emit signal_paint_cutline(Global::Any_Angle, QVariant());
        break;
    case 3:
    {
        QStringList p1 = m_start_edit->text().split(",");
        QStringList p2 = m_end_edit->text().split(",");
        if (p1.isEmpty() || p2.isEmpty())
        {
            showWarning(this, "Warning", "No coordinate values!");
            return;
        }

        QLineF line(QPointF(p1.at(0).toDouble(), p1.at(1).toDouble()), QPointF(p2.at(0).toDouble(), p2.at(1).toDouble()));
        emit signal_paint_cutline(Global::User_Input_coord, line);
        break;
    }
    case 4:
    {
        QString angle = m_cutline_angle_edit->text();
        if (angle.isEmpty())
        {
            showWarning(this, "Warning", "No Angle value!");
            return;
        }

        emit signal_paint_cutline(Global::User_Input_Angle, angle.toDouble());
        break;
    }
    default:
        break;
    }
}

void RtsReviewDialog::slot_cutline_mode_change(int index)
{
    switch (index) {
    case 0:
        m_cutline_angle_edit->setEnabled(false);
        m_start_edit->setEnabled(false);
        m_end_edit->setEnabled(false);
        break;
    case 1:
        m_cutline_angle_edit->setEnabled(false);
        m_start_edit->setEnabled(false);
        m_end_edit->setEnabled(false);

        break;
    case 2:
        m_cutline_angle_edit->setEnabled(false);
        m_start_edit->setEnabled(false);
        m_end_edit->setEnabled(false);

        break;
    case 3:
        m_cutline_angle_edit->setEnabled(false);
        m_start_edit->setEnabled(true);
        m_end_edit->setEnabled(true);

        break;
    case 4:
        m_cutline_angle_edit->setEnabled(true);
        m_start_edit->setEnabled(false);
        m_end_edit->setEnabled(false);

        break;
    default:
        break;
    }
}

void RtsReviewDialog::slot_history_table_double(QModelIndex index)
{
    QString x = index.sibling(index.row(), 1).data().toString();
    QString y = index.sibling(index.row(), 2).data().toString();
    QString range = index.sibling(index.row(), 3).data().toString();
    emit signal_set_pos_range(x, y, range);
}

void RtsReviewDialog::slot_deleta_cutline()
{
    if (m_cutline_table->currentIndex().isValid())
    {
        m_cutline_table->delete_data(m_cutline_table->currentIndex().row());
    }
    else
    {
        m_cutline_table->delete_data(m_cutline_table->count() - 1);
    }
    emit signal_updata_cutline_list(m_cutline_table->cutline_list());
}

void RtsReviewDialog::slot_delete_all_cutline()
{
    m_cutline_table->delete_all_data();
    emit signal_updata_cutline_list(m_cutline_table->cutline_list());
}

void RtsReviewDialog::slot_update_image(int index)
{
    if (index == -1)
    {
        return;
    }
    m_current_image_path = m_current_image_list.at(index);
    m_mainwindow->enable_rts_image(m_current_image_path);
}

void RtsReviewDialog::slot_update_rts_setup(int index)
{
    if (index == -1)
    {
        return;
    }
    updata_image_setup();
}

void RtsReviewDialog::init_ui()
{
    QVBoxLayout *AllVlayout = new QVBoxLayout(this);
    QVBoxLayout *TopVlayout = new QVBoxLayout();
    QHBoxLayout *RtsHLayout = new QHBoxLayout();
    m_rts_history_label = new QLabel("RTS History: ", this);
    m_rts_del_button = new QPushButton("Del", this);
    m_rts_delall_button = new QPushButton("Del All", this);
    QStringList m_history_header_list;
    m_history_header_list << "Index" << "X" << "Y" << "Size";
    m_rts_history_table = new RtsReviewHistoryTable(this, m_history_header_list);
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
    m_cutline_mode_combox->addItem("H/V");
    m_cutline_mode_combox->addItem("H/V/D");
    m_cutline_mode_combox->addItem("Any angle");
    m_cutline_mode_combox->addItem("User input coordinate");
    m_cutline_mode_combox->addItem("User input angle");
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
    QStringList cutlint_header_list;
    cutlint_header_list << "Index" << "Start" << "End" << "Center";
    m_cutline_table = new RtsReviewTable(this, cutlint_header_list);
    Bottom_Vlayout->addWidget(m_cutline_table);
    AllVlayout->addLayout(Bottom_Vlayout);
    setLayout(AllVlayout);

    m_rtscurve_widget = new RtsCurve(this);
    m_rtscurve_widget->resize(800, 600);
    m_rtscurve_widget->hide();
}

void RtsReviewDialog::init_connection()
{
    connect(m_rts_del_button, SIGNAL(clicked()), this, SLOT(slot_delete_index()));
    connect(m_rts_delall_button, SIGNAL(clicked()), this, SLOT(slot_delete_all_index()));
    connect(m_cutline_mode_button, SIGNAL(clicked()), this, SLOT(slot_draw_cutline()));
    connect(m_cutline_mode_combox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cutline_mode_change(int)));
    connect(m_rts_history_table, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_history_table_double(QModelIndex)));
    connect(m_cutline_del_button, SIGNAL(clicked()), SLOT(slot_deleta_cutline()));
    connect(m_cutline_delall_button, SIGNAL(clicked()), SLOT(slot_delete_all_cutline()));
    connect(m_display_image_combox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_update_image(int)));
    connect(m_setup_combox, SIGNAL(currentIndexChanged(int)), SLOT(slot_update_rts_setup(int)));
}

void RtsReviewDialog::parse_image()
{
    m_imagedata_worker->set_image_path(m_current_image_path);
    m_imagedata_worker->start();
}
}
