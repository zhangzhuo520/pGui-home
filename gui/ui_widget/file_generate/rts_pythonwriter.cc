#include "rts_pythonwriter.h"

namespace ui {

RtsPythonWriter::RtsPythonWriter(const RtsSetupData *data)
{
    m_binary_file =  data->binary_file;
    m_delta_defocus = data->delta_defocus;
    m_delta_dose = data->delta_dose;
    m_mask_bias = data->mask_bias;
    m_use_gpu_or_cup = data->use_gpu_or_cup;
    m_model_path = data->model_path;

    for(int i = 0; i < data->mask_table_data.count(); i ++)
    {
        m_mask_vector.append(data->mask_table_data.at(i));
    }
}

QString RtsPythonWriter::text_string()
{
    QString input_gds_string = input_gds();
    QString model_dir =     "    input_model_dir = '" + m_model_path + "'\n";
    QString chip_boundary = "    chip_boundary = [" +  QString::number(m_canvas_letf) + ", " + QString::number(m_canvas_bottom) + ", " + \
    QString::number(m_canvas_right) + ", " + QString::number(m_canvas_top) + "]  # this should be canvas coordinate\n";
    QString pitch_size = "    patch_size = " + QString::number(m_pitch_size) + "\n";
    QString input_layout_string = input_layout();
    QString get_layout_data = gds_layout();

    QString all_data_string =
            "# ---------------RTS job setup -----------------------\n"
            "''' RTS setup'''\n"
            "import re\n"
            "import os\n"
            "import gc\n"
            "import picatho.app.sutil as psu\n"
            "import picatho.app.util as pu\n"

            "def get_conditions():\n"
            "    '''Prepare te optimization conditions for concrete application commands'''\n"
            "    conditions = {}\n"
            "    conditions['Nominal_Condition'] = [0, 0, 0]\n"
            "    return conditions\n"
  "\n"
            "# -------------------- Execution Functions -------------------------\n"
            "'''The acutal work flow executed by binary'''\n"
            "def initflow():\n"
            "    '''Called before patch cutting, initialization for the whole work flow'''\n"

+           input_gds_string
+  "\n"
+           chip_boundary
+           pitch_size
+  "\n"
+           model_dir
+           "    prep_dir = './prepdb'\n"
  "\n"
            "    # set up the initialization flow based on utilty functions\n"
            "    conditions = get_conditions()\n"
            "    modeloptions = {}\n"
            "    psu.set_model(input_model_dir, conditions, modeloptions)\n"
  "\n"
            "    # Specify the input layout\n"
+           input_layout_string
+ "\n"
            "    psu.set_chip_box(*chip_boundary)\n"
            "    dbu = psu.get_layout_dbu('mainlayout')\n"
            "    psu.set_dbu(dbu)\n"
            "    psu.set_patch_size(patch_size)\n"
            "    # Specify the output layout\n"
            "    psu.set_output_layout('outputlayout', 'rts_output.oas', dbu)\n"
  "\n"
            "def process():\n"
            "    '''Actual working flow for each patch'''\n"
            "    gc.enable()\n"
  "\n"
            "    mainlayout = pu.get_input_layout_handle('mainlayout')\n"
            "    otherlayout = pu.get_input_layout_handle('otherlayout')\n"
  "\n"
            "    outlayout = pu.get_output_layout_handle('outputlayout')\n"
  "\n"
            "    # read in layer from pre-defined layout in initflow\n"
            "    a = mainlayout.read(0, 0).merge()\n"
            "    b = mainlayout.read(2, 0).merge()\n"
            "    c = mainlayout.read(3, 0).merge()\n"
            "    ## boolean for main\n"
            "    main = (a + 2) + (b-3) - c & b + a | b + a ^ b\n"
  "\n"
            "    sraf = mainlayout.read(2, 0).merge()\n"
  "\n"
            "    srif = mainlayout.read(3, 0).merge()\n"
  "\n"
            "    dummy = mainlayout.read(4, 0).merge()\n"
  "\n"
            "    # do simulation\n"
            "    simresults = pu.simulate({'main': main, 'sraf': sraf, 'srif': srif, 'dummy': dummy}, ['Nominal_Condition'], ['aerial_image', 'aerial_contour', 'resist_image', 'resist_contour'])\n"
            "    resist_contour = simresults['Nominal_Condition']['resist_contour']\n"
            "    outlayout.write(resist_contour, 100, 0, 'resist_contour')\n"
  "\n"
            "    nc_aerial_contour = simresults['Nominal_Condition']['aerial_contour']\n"
            "    outlayout.write(nc_aerial_contour, 100, 10, 'nc_aerial_contour')\n"
  "\n"
            "    resist_image = simresults['Nominal_Condition']['resist_image']\n"
            "    resist_image.dump('rts1_nc_resist_image.txt')  ## support txt/jpg/vtk format\n"
  "\n"
            "    aerial_image = simresults['Nominal_Condition']['aerial_image']\n"
            "    aerial_image.dump('rts1_nc_aerial_image.txt')  ## support txt/jpg/vtk format\n"
  "\n"
            "    gc.collect()\n"
  "\n"
  "\n"
            "if vm_status == 'initialization':\n"
            "    initflow()\n"
            "elif vm_status == 'process':\n"
            "       process()\n";
    return all_data_string;
}


void RtsPythonWriter::save_to_file()
{
    QString rts_python_path = QDir::homePath() + "/.picasso_gui" + "/pgui_rts";
    QString file_data = text_string();
    QDir dir(rts_python_path);

    if (!dir.exists())
    {
        if(!dir.mkpath(rts_python_path))
        {
            qDebug() << "make rts_python_path error !";
            return;
        }
    }

    QString filePath = rts_python_path + "/rts.py";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    QTextStream in(&file);
    in << file_data << '\n';
    file.flush();
    file.close();
}

void RtsPythonWriter::set_canvas_pos(const double & left, const double & right, const double &bottom, const double & top)
{
    m_canvas_letf = left;
    m_canvas_right = right;
    m_canvas_bottom = bottom;
    m_canvas_top = top;
    m_pitch_size = m_canvas_right - m_canvas_right;
    if (m_pitch_size > 30)
    {
        m_pitch_size = 30;
    }
    else if (m_pitch_size < 0)
    {
        logger_console << "calculate pitch size error!";
    }
}


QString RtsPythonWriter::input_gds()
{
    QStringList AllInputList;
    for (int i = 0; i < m_mask_vector.count(); i ++)
    {
        for (int j = 0; j < m_mask_vector.at(i).mask_layerdata.count(); i ++)
        {
            QString  string = m_mask_vector.at(i).mask_layerdata.at(j).layer_data;
            QStringList templist = string.split(QRegExp("\\s+|\t"), QString::SkipEmptyParts);
            if (templist.count() > 1)
            {
                AllInputList.append(templist.at(0));
            }
        }
    }

    QString gds_string = AllInputList.at(0);
    valid_gds_list.append(gds_string);

    for(int i = 0; i < AllInputList.count(); i ++)
    {
        bool is_exits = false;
        for (int j = 0; j < valid_gds_list.count(); i ++)
        {
            if (AllInputList.at(i) == valid_gds_list.at(j))
            {
                is_exits = true;
            }
        }
        if (!is_exits)
        {
            valid_gds_list.append(AllInputList.at(i));
        }
    }

    QString gds;
    for (int i = 0; i < valid_gds_list.count(); i ++)
    {
        gds += "    input_gds_" + QString::number(i) + " = " + "'" + valid_gds_list.at(i) + "'" + "\n";
    }

    return gds;
}

QString RtsPythonWriter::input_layout()
{
    QString input_layout_string;
    for (int i = 0; i < valid_gds_list.count(); i ++)
    {
        input_layout_string +=    "    psu.set_input_layout('mainlayout', input_gds_" + QString::number(i) +", preprocess_setting={'dir': prep_dir, 'mode': 'auto'})\n";
    }
    return input_layout_string;
}

QString RtsPythonWriter::gds_layout()
{
    QString gds_string =  "    # read in layer from pre-defined layout in initflow\n";
    for (int i = 0; i < m_mask_vector.count(); i ++)
    {
        QString mask_name = m_mask_vector.at(i).mask_name;
        QString boolean = m_mask_vector.at(i).boolean;
        for (int j = 0; j < m_mask_vector.at(i).mask_layerdata.count(); i ++)
        {
            QStringList templist = m_mask_vector.at(i).mask_layerdata.at(j).layer_data.split(QRegExp("\\s+|\t"), QString::SkipEmptyParts);
            if (templist.count() > 1)
            {
                QString s = templist.at(1);
                qDebug() << s;
            }
            QString  data_string = m_mask_vector.at(i).mask_layerdata.at(j).layer_data;
            QString  alias_string = m_mask_vector.at(i).mask_layerdata.at(j).alias;
        }
    }

//    "    a = mainlayout.read(0, 0).merge()\n"
//    "    b = mainlayout.read(2, 0).merge()\n"
//    "    c = mainlayout.read(3, 0).merge()\n"
//    "    ## boolean for main\n"
//    "    main = (a + 2) + (b-3) - c & b + a | b + a ^ b\n"
//"\n"
//    "    sraf = mainlayout.read(2, 0).merge()\n"
//"\n"
//    "    srif = mainlayout.read(3, 0).merge()\n"
//"\n"
//    "    dummy = mainlayout.read(4, 0).merge()\n"
    return gds_string;
}

}
