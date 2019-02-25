#ifndef UI_RTSREVIEW_WIDGET_H
#define UI_RTSREVIEW_WIDGET_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>

#include "ui_rtsreview_table.h"

namespace ui {

class RtsReviewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RtsReviewDialog(QWidget * parent = 0);
    ~RtsReviewDialog();

    void init_ui();

private:
    QLabel *m_rts_history_label;
    QPushButton *m_rts_del_button;
    QPushButton *m_rts_delall_button;
    RtsReviewTable *m_rts_history_table;


    QLabel *m_rts_setup_label;
    QLabel *m_display_image_label;
    QComboBox *m_setup_combox;
    QComboBox *m_display_image_combox;

    QLabel * m_display_contour_label;
    QCheckBox * m_ai_contour_checkbox;
    QComboBox * m_ai_color_combox;
    QLabel * m_ai_color_label;

    QCheckBox * m_ri_contour_checkbox;
    QComboBox * m_ri_color_combox;
    QLabel * m_ri_color_label;
    QCheckBox * m_ei_contour_checkbox;
    QComboBox * m_ei_color_combox;
    QLabel * m_ei_color_label;

    QLabel * m_cutline_mode_label;
    QComboBox * m_cutline_mode_combox;
    QPushButton *m_cutline_mode_button;

    QLabel *m_cutline_coord_lable;
    QLabel *m_start_label;
    QLineEdit *m_start_edit;
    QLabel *m_end_label;
    QLineEdit *m_end_edit;

    QLabel *m_cutline_angle_label;
    QLineEdit * m_cutline_angle_edit;

    QLabel *m_cutline_history_label;
    QPushButton *m_cutline_del_button;
    QPushButton *m_cutline_delall_button;

    RtsReviewTable *m_cutline_table;
};




}

#endif // UI_RTSREVIEW_WIDGET_H
