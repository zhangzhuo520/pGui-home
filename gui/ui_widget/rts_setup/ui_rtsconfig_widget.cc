#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QGroupBox>
#include <QFileDialog>
#include <QFile>

#include "../db/sqlmanager.h"
#include "../deftools/global.h"
#include "../fileparsing/yamlparsing.h"
#include "../fileparsing/rts_imageparsing.h"
#include "../file_generate/rts_pythonwriter.h"
#include "../deftools/global.h"
#include "../deftools/cmessagebox.h"
#include "rts_runprocess.h"
#include "ui_rtsconfig_widget.h"
#include "ui_rtsmask_tab.h"
namespace ui {

RtsconfigWidget::RtsconfigWidget(int rts_index, QWidget *parent):
    QWidget(parent),
    m_rts_index(rts_index),
    m_canvas_left(0),
    m_canvas_right(0),
    m_canvas_top(0),
    m_canvas_bottom(0),
    m_rts_history(0)
{
    initRtsWidget();
    initConnecttion();
    initButtonConfig();
    initProcess();
    setAttribute(Qt::WA_DeleteOnClose);
}

RtsconfigWidget::RtsconfigWidget(const RtsconfigWidget & obj, int rts_index):
    m_rts_index(rts_index),
    m_canvas_left(0),
    m_canvas_right(0),
    m_canvas_top(0),
    m_canvas_bottom(0),
    m_rts_history(0)
{
    initRtsWidget();
    initConnecttion();
    initButtonConfig();
    initProcess();
    setAttribute(Qt::WA_DeleteOnClose);

    if(obj.m_gds_radiobutton->isChecked())
    {
        this->m_gds_radiobutton->setChecked(true);
    }

    if(obj.m_job_radiobutton->isChecked())
    {
        this->m_job_radiobutton->setChecked(true);
    }

    for(int i = 0; i < obj.m_job_commbox->count(); i ++)
    {
        this->m_job_commbox->addItem(obj.m_job_commbox->itemText(i));
    }
    this->m_job_commbox->setCurrentIndex(obj.m_job_commbox->currentIndex());

    for (int i = 0; i < obj.m_model_commbox->count(); i ++)
    {
        this->m_model_commbox->addItem(obj.m_model_commbox->itemText(i));
    }
    this->m_model_commbox->setCurrentIndex(obj.m_model_commbox->currentIndex());

    *(this->m_mask_tab) = *(obj.m_mask_tab);

    this->m_maskbias_eidt->setText(obj.m_maskbias_eidt->text());
    this->m_deltadose_edit->setText(obj.m_deltadose_edit->text());
    if (obj.m_usecpu_radiobutton->isChecked())
    {
        this->m_usecpu_radiobutton->setChecked(true);
        this->m_usegpu_radiobutton->setChecked(false);
    }
    else
    {
        this->m_usecpu_radiobutton->setChecked(false);
        this->m_usegpu_radiobutton->setChecked(true);
    }
    for(int i = 0; i < obj.m_binarypath_commbox->count(); i ++)
    {
        this->m_binarypath_commbox->addItem(obj.m_binarypath_commbox->itemText(i));
    }
    this->m_binarypath_commbox->setCurrentIndex(obj.m_binarypath_commbox->currentIndex());

    for(int i = 0; i < obj.m_defocus_commbox->count(); i ++)
    {
        this->m_defocus_commbox->addItem(obj.m_defocus_commbox->itemText(i));
    }
    this->m_defocus_commbox->setCurrentIndex(obj.m_defocus_commbox->currentIndex());
}

RtsconfigWidget::~RtsconfigWidget()
{
}

void RtsconfigWidget::initRtsWidget()
{
    m_vlayout = new QVBoxLayout(this);

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
    m_vlayout->addWidget(m_mode_groupbox);

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
    MaskHlayout->setSpacing(0);
    MaskHlayout->setContentsMargins(0, 0, 0, 0);
    MaskHlayout->setStretch(0, 1);
    MaskHlayout->setStretch(1, 8);
    SetupVlayout->addLayout(MaskHlayout);
    m_setup_groupbox->setLayout(SetupVlayout);
    m_vlayout->addWidget(m_setup_groupbox);

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
    m_vlayout->addWidget(m_option_groupbox);
}

void RtsconfigWidget::initConnecttion()
{
    connect(m_model_button, SIGNAL(clicked()), this, SLOT(slot_model_browser()));
    connect(m_job_radiobutton, SIGNAL(toggled(bool)), this, SLOT(slot_job_radiobutton(bool)));
    connect(m_gds_radiobutton, SIGNAL(toggled(bool)), this, SLOT(slot_gds_radiobutton(bool)));
    connect(m_binarypath_button, SIGNAL(clicked()), this, SLOT(slot_binarypath_button()));
}

void RtsconfigWidget::initButtonConfig()
{
    m_usecpu_radiobutton->setChecked(true);
    m_usegpu_radiobutton->setChecked(false);
    m_gds_radiobutton->setChecked(true);
    m_gds_radiobutton->setChecked(false);
    m_maskbias_eidt->setText("0.0");
    m_deltadose_edit->setText("0.0");
}

void RtsconfigWidget::initSqlmannager()
{

}

void RtsconfigWidget::initProcess()
{
    m_rts_process = new RtsRunProcess(this);
    connect(m_rts_process, SIGNAL(signal_rtsrun_finished()), this, SLOT(slot_process_finished()));
    connect(m_rts_process, SIGNAL(signal_rtsrun_error(QString)), this, SLOT(slot_process_error(QString)));
    m_image_parsing = new RtsImageParsing(this);
    connect(m_image_parsing, SIGNAL(signal_parsing_finished()), this, SLOT(slot_image_parse_finished()));
}

void RtsconfigWidget::read_yaml(QString yamlPath)
{
    QFile file(yamlPath);
    if (file.exists())
    {
        YamlParsing yamlParser;
        yamlParser.read_yaml(yamlPath);
        QStringList LayerNameList = yamlParser.get_layername_list();
        m_mask_tab->init_tab(LayerNameList);
    }
    else
    {
        logger_widget("yaml file not exists!");
    }
}

void RtsconfigWidget::set_layername_list(const QStringList &list)
{
    m_layerdata_list = list;
    m_mask_tab->set_layername_list(list);
}

void RtsconfigWidget::update_job_commbox(const QStringList & list)
{
    m_job_commbox->clear();
    m_job_commbox->addItems(list);
}

void RtsconfigWidget::gds_or_job_selection()
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

void RtsconfigWidget::set_canvas_pos(const double &left, const double &right, const double &top, const double &bottom)
{
    m_canvas_left = left;
    m_canvas_right = right;
    m_canvas_bottom = bottom;
    m_canvas_top = top;
}

void RtsconfigWidget::do_apply()
{
    if (!save_setup_data())
    {
        return;
    }
    else
    {
        data_to_file();
    }
}

void RtsconfigWidget::start(const int& history_num)
{
    m_rts_history = history_num;
    QString path = QDir::homePath() + "/.pangen_gui/pgui_rts/" + "History" + QString::number(m_rts_history - 1) +  "/rts" + QString::number(m_rts_index);
    Global::delet_dir(path + "/images");
    m_rts_process->set_script_path(path);
    m_rts_process->run();
}

RtsReviewInfo RtsconfigWidget::get_rts_reviewinfo()
{
    RtsReviewInfo info;
    info.index = m_rts_index;
    info.x = QString::number(qAbs((m_canvas_right + m_canvas_left) / 2));
    info.y = QString::number(qAbs((m_canvas_top + m_canvas_bottom) / 2));
    info.output_path = QDir::homePath() + "/.pangen_gui/pgui_rts/" + "History" + QString::number(m_rts_history - 1) + "/rts" + QString::number(m_rts_index) + "/rts_output.oas";
    double range = m_canvas_right - m_canvas_left;
    if (range > 30)
    {
        range = 30;
    }
    else if (range < 0)
    {
        logger_console << "calculate pitch size error!";
    }
    info.range = QString::number(qAbs(range));
    return info;
}

RtsconfigWidget *RtsconfigWidget::clone(int index)
{
    return new RtsconfigWidget(*this, index);
}

void RtsconfigWidget::slot_model_browser()
{
    QString path = "/nfs/data/model/mask_opt/test_pattern_model_pw/result/";
    QString file = QFileDialog::getExistingDirectory(this, "Open Model File", path);
    if (file.isEmpty())
    {
        return;
    }
    else
    {
        slot_read_model(file);
    }
}

void RtsconfigWidget::slot_read_model(QString FilePath)
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
         m_model_commbox->setCurrentIndex(m_model_commbox->count() - 1);
         read_yaml(FilePath + "/Model_0/model.yaml");
    }
}

void RtsconfigWidget::slot_get_bianry_path(QString path)
{
    m_binarypath_commbox->addItem(path);
    m_binarypath_commbox->setCurrentIndex(m_binarypath_commbox->count() - 1);
}

void RtsconfigWidget::slot_job_radiobutton(bool ischecked)
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

void RtsconfigWidget::slot_gds_radiobutton(bool ischecked)
{
    if (ischecked)
    {
       m_job_radiobutton->setChecked(false);
    }
    else
    {
        m_job_radiobutton->setChecked(true);
    }
}

void RtsconfigWidget::slot_binarypath_button()
{
    QString file = QFileDialog::getOpenFileName(this, "Open Bianry File", QCoreApplication::applicationDirPath() /*+ "/work/pangen/build/bin/"*/);
    if (file.isEmpty())
    {
        return;
    }
    else
    {
        slot_get_bianry_path(file);
    }
}

void RtsconfigWidget::slot_process_finished()
{
    logger_widget("Rts :" + QString::number(m_rts_index) + ", run successfully!");
    QString path = QDir::homePath() + "/.pangen_gui/pgui_rts/" + "History" + QString::number(m_rts_history - 1) + "/rts" + QString::number(m_rts_index);
    m_image_parsing->parsing_file(path);
}

void RtsconfigWidget::slot_process_error(const QString & s)
{
    emit signal_process_error(s);
}

void RtsconfigWidget::slot_image_parse_finished()
{
    emit signal_image_parse_finished(m_rts_index);
}

void RtsconfigWidget::get_model(const QString& jobpath)
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
    QString temp_modepath;
    while (query.next())
    {
       temp_modepath = query.value(1).toString();
       break;
    }

    QString model_path;
    QStringList list = temp_modepath.split("/");
    if (list.count() < 3)
    {
        logger_widget(QString("Model path error : 1%").arg(temp_modepath));
        return;
    }
    for (int i = 0; i < list.count() - 2; i ++)
    {
        model_path += list.at(i) + "/";
    }
    model_path = model_path.left(model_path.count() - 1);

    m_sqlmannager->closeDB();
    bool is_model_exist(false);
    for (int i = 0; i < m_model_commbox->count(); i ++)
    {
        if (model_path == m_model_commbox->itemText(i))
        {
            is_model_exist = true;
        }
    }

    if (!is_model_exist)
    {
        m_model_commbox->addItem(model_path);
    }

    slot_read_model(model_path);
    delete m_sqlmannager;
    m_sqlmannager = NULL;
}

bool RtsconfigWidget::save_setup_data()
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

    if (m_mask_tab->count() == 0)
    {
        QString waring_string = QString("%1 No Mask Data!").arg(m_mask_data.mask_name);
        showWarning(this, "Waring", waring_string);
        return false;
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
//        if (m_mask_data.mask_layerdata.isEmpty())
//        {
//            QString waring_string = QString("%1 No select layerdata!").arg(m_mask_data.mask_name);
//            showWarning(this, "Waring", waring_string);
//            return false;
//        }

        if (m_mask_data.boolean.isEmpty() && (data_list.count() > 1))
        {
            QString waring_string = QString("%1 No input boolean!").arg(m_mask_data.mask_name);
            showWarning(this, "Waring", waring_string);
            return false;
        }
        m_setup_data.mask_table_data.append(m_mask_data);
        m_mask_data.mask_layerdata.clear();
    }

    return true;
}

void RtsconfigWidget::data_to_file()
{
    if (m_canvas_left == 0 && m_canvas_right == 0 && m_canvas_bottom == 0 && m_canvas_top == 0)
    {
        logger_widget("the canvas pos not setting!");
        return;
    }
    RtsPythonWriter rtswriter(&m_setup_data);
    rtswriter.set_canvas_pos(m_canvas_left, m_canvas_right, m_canvas_bottom, m_canvas_top);
    QString path = QDir::homePath() + "/.pangen_gui/pgui_rts/" + "History" + QString::number(m_rts_history) + "/rts" + QString::number(m_rts_index);
    rtswriter.save_to_file(path);
}
}
