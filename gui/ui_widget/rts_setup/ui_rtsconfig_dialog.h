#ifndef RTSCONFIGWIDGET_H
#define RTSCONFIGWIDGET_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QFrame>
#include <QGroupBox>
#include <QFileDialog>
#include <QFile>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#include "ui_rtsmask_tab.h"
#include "../db/sqlmanager.h"
#include "../deftools/defcontrols.h"
#include "../deftools/global.h"
#include "../deftools/datastruct.h"
#include "../fileparsing/yamlparsing.h"
#include "../qt_logger/pgui_log_global.h"
#include "../file_generate/rts_pythonwriter.h"

const int LableWidth = 140;
const int LableTitilWidth = 50;
const int pushButtonWidth = 40;
namespace ui {
class RtsConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RtsConfigDialog(QWidget *parent = 0);

    ~RtsConfigDialog();

    void initRtsWidget();

    void initTopButton();

    void initTabWidget();

    void initBottomButton();

    void initLayout();

    void initConnecttion();

    void initButtonConfig();

    void initSqlmannager();

    void initRtsTab(const QStringList &);

    void read_yaml(QString);

    void set_layername_list(const QStringList &);

    void update_job_commbox(const QStringList &);

    void gds_or_job_selection();
    
public slots:
    void slotAddRts();

    void slotDeleteRts();

    void slotCloneRts();

    void slot_model_browser();

    void slot_read_model(QString);

    void slot_job_radiobutton(bool);

    void slot_gds_radiobutton(bool);

    void slot_ok_button();

    void slot_cancel_button();

    void slot_apply_button();
private:
    void get_model(const QString &);

    void save_setup_data();

    void data_to_file();

    QPushButton *m_add_button;
    QPushButton *m_clone_button;
    QPushButton *m_delete_button;

    QTabWidget *m_rts_tab;
    QVBoxLayout *m_tab_vlayout;

    QWidget *m_rts_widget;
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

    QPushButton *m_cancel_button;
    QPushButton *m_ok_button;
    QPushButton *m_apply_button;

    QVBoxLayout *Vlayout;
    QHBoxLayout *Hlayout;

    QFileDialog *m_model_dialog;
    SQLManager *m_sqlmannager;
    RtsSetupData m_setup_data;
    RtsLayerData m_layer_data;
    RtsMaskData m_mask_data;
};
}
#endif // RTSCONFIGWIDGET_H
