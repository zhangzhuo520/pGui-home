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
    initSqlmannager();
    initConnecttion();
    initButtonConfig();
    initFileDialog();
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
    m_gds_radiobutton = new QRadioButton("RTS on GDS", m_mode_groupbox);
    m_job_radiobutton = new QRadioButton("RTS on Verification", m_mode_groupbox);
    m_job_radiobutton->setMaximumWidth(LableWidth);
    m_job_commbox = new QComboBox(m_mode_groupbox);
    ModeHLayout->setSpacing(0);
    ModeHLayout->setContentsMargins(0, 0, 0, 0);
    ModeHLayout->addWidget(m_job_radiobutton);
    ModeHLayout->addWidget(m_job_commbox);

    ModeVLayout->addWidget(m_gds_radiobutton);
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
    m_binarypath_commbox = new QComboBox(m_option_groupbox);
    m_binarypath_button = new QPushButton("...",m_option_groupbox);
    m_binarypath_button->setFixedWidth(pushButtonWidth);
    OptionHlayout->addWidget(label7);
    OptionHlayout->addWidget(m_binarypath_commbox);
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
    m_rts_tab = new RtsTabWidget(this);
    m_rts_tab->setObjectName("RtsTabWidget");
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

void RtsConfigDialog::initFileDialog()
{
    m_model_dialog = new QFileDialog(this);
    m_model_dialog->setWindowModality(Qt::ApplicationModal);
    m_model_dialog->setWindowTitle(tr("Open Layout File"));
    m_model_dialog->setDirectory(QDir::homePath());
    m_model_dialog->setNameFilter(tr("Directories"));
    connect(m_model_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_read_model(QString)), Qt::UniqueConnection);
    m_model_dialog->setFileMode(QFileDialog::Directory);
    m_model_dialog->setViewMode(QFileDialog::List);

    m_bianry_dialog = new QFileDialog(this);
    m_bianry_dialog->setWindowModality(Qt::ApplicationModal);
    m_bianry_dialog->setWindowTitle(tr("Open Layout File"));
    m_bianry_dialog->setDirectory(QDir::homePath());
    connect(m_bianry_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_get_bianry_path(QString)), Qt::UniqueConnection);
    m_bianry_dialog->setFileMode(QFileDialog::AnyFile);
    m_bianry_dialog->setViewMode(QFileDialog::List);
}

void RtsConfigDialog::initLayout()
{
    Vlayout = new QVBoxLayout(this);
    Hlayout = new QHBoxLayout();
    Hlayout->setSpacing(10);
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->addWidget(new QLabel(""));
    Hlayout->addWidget(m_add_button);
    Hlayout->addWidget(m_delete_button);
    Hlayout->addWidget(m_clone_button);
    Hlayout->setStretch(0, 5);
    Hlayout->setStretch(1, 1);
    Hlayout->setStretch(2, 1);
    Hlayout->setStretch(3, 1);
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
    connect(m_model_button, SIGNAL(clicked()), this, SLOT(slot_model_browser()));
    connect(m_job_radiobutton, SIGNAL(toggled(bool)), this, SLOT(slot_job_radiobutton(bool)));
    connect(m_gds_radiobutton, SIGNAL(toggled(bool)), this, SLOT(slot_gds_radiobutton(bool)));
    connect(m_binarypath_button, SIGNAL(clicked()), this, SLOT(slot_binarypath_button()));
    connect(m_ok_button, SIGNAL(clicked()), this, SLOT(slot_ok_button()));
    connect(m_cancel_button, SIGNAL(clicked()), this, SLOT(slot_cancel_button()));
    connect(m_apply_button, SIGNAL(clicked()), this, SLOT(slot_apply_button()));
}

void RtsConfigDialog::initButtonConfig()
{
    m_usecpu_radiobutton->setChecked(true);
    m_usegpu_radiobutton->setChecked(false);
    m_gds_radiobutton->setChecked(true);
    m_gds_radiobutton->setChecked(false);
    m_maskbias_eidt->setText("0.0");
    m_deltadose_edit->setText("0.0");
}

void RtsConfigDialog::initSqlmannager()
{

}

void RtsConfigDialog::initRtsTab(const QStringList & TabList)
{
    m_mask_tab->init_tab(TabList);
}

void RtsConfigDialog::read_yaml(QString yamlPath)
{
    QFile file(yamlPath);
    if (file.exists())
    {
        YamlParsing yamlParser;
        yamlParser.read_yaml(yamlPath);
        QStringList LayerNameList = yamlParser.get_layername_list();
        initRtsTab(LayerNameList);
    }
    else
    {
        logger_console.error("yaml file not exists!");
    }
}

void RtsConfigDialog::set_layername_list(const QStringList &list)
{
    m_layerdata_list = list;
    m_mask_tab->set_layername_list(list);
}

void RtsConfigDialog::update_job_commbox(const QStringList & list)
{
    m_job_commbox->clear();
    m_job_commbox->addItems(list);
}

void RtsConfigDialog::gds_or_job_selection()
{
    if (m_job_radiobutton->isChecked() && (!m_job_commbox->currentText().isEmpty()))
    {
        get_model(m_job_commbox->currentText());
    }
    else
    {
        // do nothing...
    }
}

void RtsConfigDialog::set_canvas_pos(const double &left, const double &right, const double &top, const double &bottom)
{
    m_canvas_left = left;
    m_canvas_right = right;
    m_canvas_bottom = bottom;
    m_canvas_top = top;
}

void RtsConfigDialog::slotAddRts()
{
    initRtsWidget();
    initConnecttion();
    initButtonConfig();
    QString tabTitle = "rts_" + QString::number(m_rts_tab->count() + 1);
    m_mask_tab->set_layername_list(m_layerdata_list);
    m_rts_tab->addTab(m_rts_widget, tabTitle);
}

void RtsConfigDialog::slotDeleteRts()
{
    QWidget *w = m_rts_tab->widget(m_rts_tab->count() - 1);
    m_rts_tab->removeTab(m_rts_tab->count() - 1);
    delete w;
}

void RtsConfigDialog::slotCloneRts()
{
}

void RtsConfigDialog::slot_model_browser()
{
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
         m_model_commbox->addItem((FilePath));
         read_yaml(FilePath + "/Model_0/model.yaml");
    }
}

void RtsConfigDialog::slot_get_bianry_path(QString path)
{
    m_binarypath_commbox->addItem(path);
    m_binarypath_commbox->setCurrentIndex(m_binarypath_commbox->count() - 1);
}

void RtsConfigDialog::slot_job_radiobutton(bool ischecked)
{
    if (ischecked)
    {
        gds_or_job_selection();
    }
    else
    {
        //do nothing...
    }
}

void RtsConfigDialog::slot_gds_radiobutton(bool ischecked)
{
    if (ischecked)
    {
        //do nothing
    }
}

void RtsConfigDialog::slot_binarypath_button()
{
    m_bianry_dialog->show();
}

void RtsConfigDialog::slot_ok_button()
{
    if (false == save_setup_data())
    {
        return;
    }
    data_to_file();
    this->close();
}

void RtsConfigDialog::slot_cancel_button()
{
}

void RtsConfigDialog::slot_apply_button()
{
    if (false == save_setup_data())
    {
        return;
    }
    data_to_file();
}

void RtsConfigDialog::get_model(const QString& jobpath)
{
    m_sqlmannager = new SQLManager();
    QString m_db_path = jobpath + "/defectDB.sqlite";
    QFile file(m_db_path);
    if (file.exists())
    {
        m_sqlmannager->setDatabaseName(m_db_path);
    }
    else
    {
        qDebug() << "m_db_path is Empty";
    }
    if(!m_sqlmannager->openDB())
    {
        qDebug() << "DB open Failed";
    }

    QSqlQuery query;
    query.exec("select * from process");	//执行
    QString modepath;
    while (query.next())
    {
       modepath = query.value(1).toString();
       break;
    }

    m_sqlmannager->closeDB();
    bool is_model_exist(false);
    for (int i = 0; i < m_model_commbox->count(); i ++)
    {
        if (modepath == m_model_commbox->itemText(i))
        {
            is_model_exist = true;
        }
    }

    if (!is_model_exist)
    {
        m_model_commbox->addItem(modepath);
    }
    read_yaml(modepath);
    delete m_sqlmannager;
    m_sqlmannager = NULL;
}

bool RtsConfigDialog::save_setup_data()
{
    m_setup_data.model_path = m_model_commbox->currentText();
    m_setup_data.binary_file = m_binarypath_commbox->currentText();
    m_setup_data.delta_defocus = m_defocus_commbox->currentText();
    m_setup_data.delta_dose = m_deltadose_edit->text();
    m_setup_data.mask_bias = m_maskbias_eidt->text();
    m_setup_data.mask_table_data.clear();

    if (m_usecpu_radiobutton->isChecked())
    {
        m_setup_data.use_gpu_or_cup = "cpu";
    }
    else if(m_usegpu_radiobutton->isChecked())
    {
        m_setup_data.use_gpu_or_cup = "gpu";
    }
    for (int i = 0; i < m_mask_tab->count(); i ++)
    {
        QStringList alisa_list = m_mask_tab->get_alisa_list(i);
        QStringList data_list = m_mask_tab->get_layerdata_list(i);
        m_mask_data.mask_name = m_mask_tab->tabText(i);
        m_mask_data.boolean = m_mask_tab->get_boolean(i);

        if (!data_list.isEmpty())
        {
            for(int j = 0; j < data_list.count(); j ++)
            {
                m_layer_data.alias = alisa_list.at(j);
                m_layer_data.layer_data = data_list.at(j);
                m_mask_data.mask_layerdata.append(m_layer_data);
            }
        }
        else
        {
            m_mask_data.mask_layerdata.clear();
        }

        if (m_mask_data.boolean.isEmpty() && (data_list.count() > 1))
        {
            QString waring_string = QString("%1 No input boolean!").arg(m_mask_data.mask_name);
            showWarning(this, "Waring", waring_string);
            return false;
        }
        m_setup_data.mask_table_data.append(m_mask_data);
        m_mask_data.mask_layerdata.clear();
    }
//    for (int i = 0; i < m_setup_data.mask_table_data.count(); i ++)
//    {

//    }
    emit signal_get_current_canvaspos();
    return true;
}

void RtsConfigDialog::data_to_file()
{
    RtsPythonWriter rtswriter(&m_setup_data);
    rtswriter.set_canvas_pos(m_canvas_left, m_canvas_right, m_canvas_bottom, m_canvas_top);
    rtswriter.save_to_file();
}
}
