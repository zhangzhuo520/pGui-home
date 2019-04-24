#ifndef UI_RTSREVIEW_WIDGET_H
#define UI_RTSREVIEW_WIDGET_H

#include <QDialog>
#include <QDebug>
#include <QMainWindow>

#include "../deftools/global.h"
#include "ui_rtsreview_table.h"
class QLabel;
class QTableWidget;
class QPushButton;
class QComboBox;
class QCheckBox;
class QLineEdit;

namespace ui {
class RtsCurve;
class RtsReviewTable;
class ImageDataWorker;
class MainWindow;
class RtsManager;
class RtsReviewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RtsReviewDialog(QWidget * parent = 0);
    ~RtsReviewDialog();

    void append_history_data(const QString &);
    void append_setup_combox(const QString &);
    void rts_setup_clear();
    void set_cutline_table(const QList <LineData*>&);

    void append_image_commbox(const QString&);
    void clear_image_commbox();

    void updata_image_setup();

signals:
    void signal_set_pos_range(QString, QString, QString);
    void signal_paint_cutline(Global::RtsCutLineAngle, const QVariant&);
    void signal_updata_cutline_list(const QList <LineData*>&);

public slots:
    void slot_delete_index();
    void slot_delete_all_index();
    void slot_draw_cutline();
    void slot_cutline_mode_change(int);
    void slot_history_table_double(QModelIndex);
    void slot_deleta_cutline();
    void slot_delete_all_cutline();
    void slot_update_image(int);
    void slot_update_rts_setup(int);

private:
    void init_ui();
    void init_connection();
    void parse_image();

    QLabel *m_rts_history_label;
    QPushButton *m_rts_del_button;
    QPushButton *m_rts_delall_button;
    RtsReviewHistoryTable *m_rts_history_table;

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
    RtsCurve *m_rtscurve_widget;
    ImageDataWorker *m_imagedata_worker;

    MainWindow *m_mainwindow;
    RtsManager *m_rts_manager;
    QString m_current_image_path;
    QStringList m_current_image_list;
    int m_rts_count_flag;
    int m_image_count_flag;
};




}

#endif // UI_RTSREVIEW_WIDGET_H
