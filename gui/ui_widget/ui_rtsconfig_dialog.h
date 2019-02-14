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

#include "./deftools/defcontrols.h"
#include "ui_rtsmask_tab.h"
#include "deftools/global.h"
#include "fileparsing/yamlparsing.h"

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

    void initRtsTab(const QStringList &);

    void read_yaml(QString);

    void set_layername_list(const QStringList &);
    
public slots:
    void slotAddRts();

    void slotDeleteRts();

    void slotCloneRts();

    void slot_model_browser();

    void slot_read_model(QString);

private:
    QPushButton *m_add_button;
    QPushButton *m_clone_button;
    QPushButton *m_delete_button;

    QTabWidget *m_rts_tab;
    QVBoxLayout *m_tab_vlayout;

    QWidget *m_rts_widget;
    QGroupBox *m_mode_groupbox;
    QGroupBox *m_setup_groupbox;
    QGroupBox *m_option_groupbox;
    QRadioButton *m_dgs_radiobutton;
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

    QLineEdit *m_binarypath_edit;
    QPushButton *m_binarypath_button;

    QPushButton *m_cancel_button;
    QPushButton *m_ok_button;
    QPushButton *m_apply_button;

    QVBoxLayout* Vlayout;
    QHBoxLayout *Hlayout;

    QFileDialog * m_model_dialog;
};
}
#endif // RTSCONFIGWIDGET_H
