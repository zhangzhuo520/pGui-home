#include "ui_rtsconfig_dialog.h"
namespace ui {

RtsConfigDialog::RtsConfigDialog(QWidget *parent) :
    QDialog(parent)
{
    initTopButton();
    initRtsWidget();
    initTabWidget();
    initBottomButton();
    initLayout();
    initConnecttion();

}

RtsConfigDialog::~RtsConfigDialog()
{
}

void RtsConfigDialog::initRtsWidget()
{
    m_rts_widget = new QWidget(this);
    m_tab_vlayout = new QVBoxLayout(m_rts_widget);

    m_mode_groupbox = new QGroupBox("Mode");
    QVBoxLayout *ModeVLayout = new QVBoxLayout();
    QHBoxLayout *ModeHLayout = new QHBoxLayout();
    m_dgs_radiobutton = new QRadioButton("RTS on GDS", m_mode_groupbox);
    m_job_radiobutton = new QRadioButton("RTS on Verification", m_mode_groupbox);
    m_job_radiobutton->setMaximumWidth(LableWidth);
    m_job_commbox = new QComboBox(m_mode_groupbox);
    ModeHLayout->setSpacing(0);
    ModeHLayout->setContentsMargins(0, 0, 0, 0);
    ModeHLayout->addWidget(m_job_radiobutton);
    ModeHLayout->addWidget(m_job_commbox);

    ModeVLayout->addWidget(m_dgs_radiobutton);
    ModeVLayout->addLayout(ModeHLayout);
    m_mode_groupbox->setLayout(ModeVLayout);
    m_tab_vlayout->addWidget(m_mode_groupbox);

    m_setup_groupbox = new QGroupBox("Setup");
    QVBoxLayout *SetupVlayout = new QVBoxLayout();
    QHBoxLayout *SetupHlayout = new QHBoxLayout();
    QLabel *label1 = new QLabel("Model", m_setup_groupbox);
    label1->setFixedWidth(LableTitilWidth);
    m_model_commbox = new Commbox(m_setup_groupbox);
    m_model_button = new QPushButton("...",m_setup_groupbox);
    m_model_button->setFixedWidth(pushButtonWidth);
    SetupHlayout->addWidget(label1);
    SetupHlayout->addWidget(m_model_commbox);
    SetupHlayout->addWidget(m_model_button);
    SetupHlayout->setStretch(0, 2);
    SetupHlayout->setStretch(1, 9);
    SetupHlayout->setStretch(2, 1);
    SetupVlayout->addLayout(SetupHlayout);

    QHBoxLayout *MaskHlayout = new QHBoxLayout();
    QLabel *label2 = new QLabel("Mask", m_setup_groupbox);
    label2->setFixedWidth(LableTitilWidth);
    m_mask_tab = new RtsMaskTab(m_setup_groupbox);
    MaskHlayout->addWidget(label2);
    MaskHlayout->addWidget(m_mask_tab);
    MaskHlayout->setStretch(0, 1);
    MaskHlayout->setStretch(1, 5);
    SetupVlayout->addLayout(MaskHlayout);
    m_setup_groupbox->setLayout(SetupVlayout);
    m_tab_vlayout->addWidget(m_setup_groupbox);

    m_option_groupbox = new QGroupBox("Option");
    QVBoxLayout *OptionVlayout = new QVBoxLayout();
    QHBoxLayout *OptionHlayout = new QHBoxLayout();
    QLabel *label3 = new QLabel("Mask Bias(nm):     ", m_option_groupbox);
    label3->setMinimumWidth(LableWidth);
    m_maskbias_eidt = new QLineEdit(m_option_groupbox);
    OptionHlayout->addWidget(label3);
    OptionHlayout->addWidget(m_maskbias_eidt);
    OptionHlayout->setStretch(0, 1);
    OptionHlayout->setStretch(1, 8);
    OptionVlayout->addLayout(OptionHlayout);

    OptionHlayout = new QHBoxLayout();
    QLabel *label4 = new QLabel("Delta Dose(%):  ", m_option_groupbox);
    label4->setMinimumWidth(LableWidth);
    m_deltadose_edit = new QLineEdit(m_option_groupbox);

    OptionHlayout->addWidget(label4);
    OptionHlayout->addWidget(m_deltadose_edit);
    OptionHlayout->setStretch(0, 1);
    OptionHlayout->setStretch(1, 8);
    OptionVlayout->addLayout(OptionHlayout);
    OptionHlayout = new QHBoxLayout();
    QLabel *label5 = new QLabel("Delta Defocus(nm):", m_option_groupbox);
    label5->setMinimumWidth(LableWidth);
    m_defocus_commbox = new QComboBox(m_option_groupbox);
    OptionHlayout->addWidget(label5);
    OptionHlayout->addWidget(m_defocus_commbox);
    OptionHlayout->setStretch(0, 1);
    OptionHlayout->setStretch(1, 8);
    OptionVlayout->addLayout(OptionHlayout);
    OptionHlayout = new QHBoxLayout();
    m_usegpu_radiobutton = new QRadioButton(m_option_groupbox);
    m_usegpu_radiobutton->setText("Use GPU");
    m_usegpu_radiobutton->setMinimumWidth(LableWidth);
    m_usecpu_radiobutton = new QRadioButton(m_option_groupbox);
    m_usecpu_radiobutton->setText("Use CPU");
    m_usecpu_radiobutton->setMinimumWidth(LableWidth);
    OptionHlayout->addWidget(m_usegpu_radiobutton);
    OptionHlayout->addWidget(m_usecpu_radiobutton);
    OptionVlayout->addLayout(OptionHlayout);

    OptionHlayout = new QHBoxLayout();
    QLabel *label7 = new QLabel("Binary File :", m_option_groupbox);
    label7->setMinimumWidth(LableWidth);
    m_binarypath_edit = new QLineEdit(m_option_groupbox);
    m_binarypath_button = new QPushButton("...",m_option_groupbox);
    m_binarypath_button->setFixedWidth(pushButtonWidth);
    OptionHlayout->addWidget(label7);
    OptionHlayout->addWidget(m_binarypath_edit);
    OptionHlayout->addWidget(m_binarypath_button);
    OptionHlayout->setStretch(0, 1);
    OptionHlayout->setStretch(1, 5);
    OptionHlayout->setStretch(2, 1);
    OptionVlayout->addLayout(OptionHlayout);

    m_option_groupbox->setLayout(OptionVlayout);
    m_tab_vlayout->addWidget(m_option_groupbox);
}

void RtsConfigDialog::initTopButton()
{
    m_add_button = new QPushButton("Add RTS", this);
    m_clone_button = new QPushButton("Clone RTS",this);
    m_delete_button = new QPushButton("Delete RTS", this);
    connect(m_add_button, SIGNAL(clicked()), this, SLOT(slotAddRts()));
    connect(m_clone_button, SIGNAL(clicked()), this, SLOT(slotCloneRts()));
    connect(m_delete_button, SIGNAL(clicked()), this, SLOT(slotDeleteRts()));
}

void RtsConfigDialog::initTabWidget()
{
    m_rts_tab = new QTabWidget(this);
    QString tabTitle = "rts_" + QString::number(1);
    m_rts_tab->addTab(m_rts_widget, tabTitle);
}

void RtsConfigDialog::initBottomButton()
{
    m_ok_button = new QPushButton("Ok", this);
    m_cancel_button = new QPushButton("Cancel",this);
    m_apply_button = new QPushButton("Apply",this);

    connect(m_cancel_button, SIGNAL(clicked()), this, SLOT(close()));
}

void RtsConfigDialog::initLayout()
{
    Vlayout = new QVBoxLayout(this);
    Hlayout = new QHBoxLayout();
    Hlayout->setSpacing(10);
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->addWidget(m_add_button);
    Hlayout->addWidget(m_delete_button);
    Hlayout->addWidget(m_clone_button);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    Hlayout->addWidget(m_rts_tab);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    Hlayout->addSpacerItem(new QSpacerItem(200, 20, QSizePolicy::Preferred, QSizePolicy::Preferred));
    Hlayout->addWidget(m_ok_button);
    Hlayout->addWidget(m_cancel_button);
    Hlayout->addWidget(m_apply_button);
    Vlayout->addLayout(Hlayout);

    setLayout(Vlayout);
}

void RtsConfigDialog::initConnecttion()
{
    m_model_dialog = NULL;
    connect(m_model_button, SIGNAL(clicked()), this, SLOT(slot_model_browser()));
}

void RtsConfigDialog::initRtsTab(const QStringList & TabList)
{
    m_mask_tab->init_tab(TabList);
}

void RtsConfigDialog::read_yaml(QString yamlPath)
{
    YamlParsing yamlParser;
    yamlParser.read_yaml(yamlPath);
    QStringList LayerNameList = yamlParser.get_layername_list();
    initRtsTab(LayerNameList);
}

void RtsConfigDialog::slotAddRts()
{
    initRtsWidget();
    initConnecttion();
    QString tabTitle = "rts_" + QString::number(m_rts_tab->count() + 1);
    m_rts_tab->addTab(m_rts_widget, tabTitle);
}

void RtsConfigDialog::slotDeleteRts()
{
    delete m_rts_tab->widget(m_rts_tab->count() - 1);
    m_rts_tab->removeTab(m_rts_tab->count() - 1);
}

void RtsConfigDialog::slotCloneRts()
{
}

void RtsConfigDialog::slot_model_browser()
{
    if (m_model_dialog == NULL)
    {
        m_model_dialog = new QFileDialog(this);
    }
    m_model_dialog->setWindowModality(Qt::ApplicationModal);
    m_model_dialog->setWindowTitle(tr("Open Layout File"));
    m_model_dialog->setDirectory("/home/dfjy/workspace/job");
    m_model_dialog->setNameFilter(tr("Directories"));
    connect(m_model_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_read_model(QString)), Qt::UniqueConnection);
    m_model_dialog->setFileMode(QFileDialog::Directory);
    m_model_dialog->setViewMode(QFileDialog::List);
    m_model_dialog->show();
}

void RtsConfigDialog::slot_read_model(QString FilePath)
{
    m_defocus_commbox->clear();
    QDir mode_dir(FilePath);
    QFileInfoList file_list = mode_dir.entryInfoList();
    bool isModel = false;
    for (int i = 0; i < file_list.count(); i ++)
    {
        if (file_list.at(i).isDir())
        {
            QStringList  list = file_list.at(i).fileName().split("_");
            if (list.count() > 1 && list.at(0) == "Model")
            {
                isModel = true;
                m_defocus_commbox->addItem(list.at(1));
            }
        }
    }
    if (isModel)
    {
         m_job_commbox->addItem(FilePath);
         m_model_commbox->addItem((FilePath));
         read_yaml(FilePath + "/Model_0/model.yaml");
    }
}
}
