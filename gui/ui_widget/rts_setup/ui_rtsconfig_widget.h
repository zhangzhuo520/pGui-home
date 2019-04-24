#ifndef UI_RTSCONFIG_WIDGET_H
#define UI_RTSCONFIG_WIDGET_H
#include <QWidget>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QCoreApplication>


#include "../deftools/defcontrols.h"
#include "../deftools/global.h"
#include "../deftools/datastruct.h"
#include "../qt_logger/pgui_log_global.h"


class QGroupBox;
class QTableWidget;
class QLineEdit;
class QFrame;
class QLabel;
class QVBoxLayout;
class QComboBox;
class QFileDialog;
class RtsPythonWriter;
namespace ui {

const int LableWidth = 140;
const int LableTitilWidth = 50;
const int pushButtonWidth = 40;
class RtsMaskTab;
class SQLManager;
class RtsImageParsing;
class RtsRunProcess;
class RtsconfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RtsconfigWidget(int rts_index = 0, QWidget * parent = 0);
    explicit RtsconfigWidget(const RtsconfigWidget&, int);


    ~RtsconfigWidget();
    void initRtsWidget();

    void initConnecttion();

    void initButtonConfig();

    void initSqlmannager();

    void initProcess();

    void read_yaml(QString);

    void set_layername_list(const QStringList &);

    void update_job_commbox(const QStringList &);

    void gds_or_job_selection();

    void set_canvas_pos(const double&, const double&, const double&, const double&);

    void do_apply();

    void start(const int&);

    RtsReviewInfo get_rts_reviewinfo();

    RtsconfigWidget *clone(int);

public slots:
    void slot_model_browser();

    void slot_read_model(QString);

    void slot_get_bianry_path(QString);

    void slot_job_radiobutton(bool);

    void slot_gds_radiobutton(bool);

    void slot_binarypath_button();

    void slot_process_finished();

    void slot_process_error(const QString&);

    void slot_image_parse_finished();

signals:
    void signal_image_parse_finished(int);

    void signal_process_error(const QString&);

private:
    void get_model(const QString &);

    bool save_setup_data();

    void data_to_file();

    int m_rts_index;

    QGroupBox *m_mode_groupbox;
    QGroupBox *m_setup_groupbox;
    QGroupBox *m_option_groupbox;
    QRadioButton *m_gds_radiobutton;
    QRadioButton *m_job_radiobutton;
    QComboBox * m_job_commbox;

    QComboBox *m_model_commbox;
    QPushButton * m_model_button;

    QLineEdit * m_maskbias_eidt;
    QLineEdit * m_deltadose_edit;

    QComboBox * m_defocus_commbox;

    RtsMaskTab * m_mask_tab;

    QRadioButton * m_usegpu_radiobutton;
    QRadioButton * m_usecpu_radiobutton;

    QComboBox *m_binarypath_commbox;
    QPushButton *m_binarypath_button;

    QVBoxLayout *m_vlayout;

    SQLManager *m_sqlmannager;
    RtsSetupData m_setup_data;
    RtsLayerData m_layer_data;
    RtsMaskData m_mask_data;
    double m_canvas_left;
    double m_canvas_right;
    double m_canvas_top;
    double m_canvas_bottom;

    QStringList m_layerdata_list;
    RtsRunProcess *m_rts_process;
    RtsImageParsing *m_image_parsing;
    int m_rts_history;
};
}

#endif // UI_RTSCONFIG_WIDGET_H
