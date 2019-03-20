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
    QString get_layout_data_string = gds_layout();
    QString all_data_string =
            "# ---------------RTS job setup -----------------------\n"
            "''' RTS setup'''\n"
            "import re\n"
            "import os\n"
            "import gc\n"
            "import pangen.app.sutil as psu\n"
            "import pangen.app.util as pu\n"

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
+           get_layout_data_string
+ "\n"
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
    create_pframe_file();
    create_run_file();
    QString rts_python_path = QDir::homePath() + "/.pangen_gui" + "/pgui_rts";
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
    m_pitch_size = m_canvas_right - m_canvas_letf;
    if (m_pitch_size > 30)
    {
        m_pitch_size = 30;
    }
    else if (m_pitch_size < 0)
    {
        logger_console << "calculate pitch size error!";
    }
}

void RtsPythonWriter::create_pframe_file()
{
    QString rts_python_path = QDir::homePath() + "/.pangen_gui" + "/pgui_rts";
    QString use_gpu = get_gpu();
    QString file_data =
            "import pangen.system as psys\n"\
            "\n"\
            "def mainflow():\n"\
            "    options = {}\n"\
            "    options['python_file'] = \"rts.py\"\n"\
            "    options['use_gpu'] = \"" + use_gpu + "\"\n"\
            "    options['preprocess_thread_count'] = 2\n"\
            "    psys.execute_session('optimization', mode = \"singlethread\", session_name = 'rts', options = options)\n"\
            "\n"\
            "\n"\
            "\n"\
            "if __name__ == \"__main__\":\n"\
            "   psys.info('job beginning >>>>>>>>>>>>>>>>>')\n"\
            "   mainflow()\n"\
            "   pass\n";
    QDir dir(rts_python_path);

    if (!dir.exists())
    {
        if(!dir.mkpath(rts_python_path))
        {
            logger_console << "make rts_python_path error !";
            return;
        }
    }

    QString filePath = rts_python_path + "/pframe.py";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    QTextStream in(&file);
    in << file_data << '\n';
    file.flush();
    file.close();
}

void RtsPythonWriter::create_run_file()
{
    QString rts_python_path = QDir::homePath() + "/.pangen_gui" + "/pgui_rts";
    QStringList list = m_binary_file.split("bin");
    if (list.count() != 2)
    {
     //   qDebug() << QString("rts pangen path error!(%1)").arg(list);
        return;
    }
    QString pangen_path = list.at(0) + "script";

    QString file_data =
        "#!/bin/bash\n"\
        "source ~/.bash_profile\n"
        "#ulimit -c unlimited\n"
        "pangenpath='" + pangen_path + "'\n"
        "export PYTHONPATH=$PYTHONPATH:$pangenpath\n"
+       m_binary_file + " -script pframe.py;\n";
    QDir dir(rts_python_path);

    if (!dir.exists())
    {
        if(!dir.mkpath(rts_python_path))
        {
            logger_console << "make rts_python_path error !";
            return;
        }
    }

    QString filePath = rts_python_path + "/run.sh";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    QTextStream in(&file);
    in << file_data << '\n';
    file.flush();
    file.close();
}

QString RtsPythonWriter::input_gds()
{
    QStringList AllInputList;
    for (int i = 0; i < m_mask_vector.count(); i ++)
    {
        for (int j = 0; j < m_mask_vector.at(i).mask_layerdata.count(); j ++)
        {

            QString  string = m_mask_vector.at(i).mask_layerdata.at(j).layer_data;
            QStringList templist = string.split(QRegExp("\\s+|\t"), QString::SkipEmptyParts);
            if (templist.count() > 1)
            {
                AllInputList.append(templist.at(0));
            }
            else
            {
                qDebug() << "Tabel layer data is Empty ! data: " << templist;
            }
        }
    }
    if (AllInputList.count() < 1)
    {
        qDebug() << "InputList is Empty!" << AllInputList;
        return QString();
    }

    QString gds_string = AllInputList.at(0);
    valid_gds_list.append(gds_string);
    for(int i = 0; i < AllInputList.count(); i ++)
    {
        bool is_exits = false;
        for (int j = 0; j < valid_gds_list.count(); j ++)
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
    QString layout_data_string;
    for (int i = 0; i < m_mask_vector.count(); i ++)
    {
        layout_data_string.clear();
        QString mask_name = m_mask_vector.at(i).mask_name;
        QString boolean = QString("    %1 = %2\n").arg(mask_name).arg(m_mask_vector.at(i).boolean);
        if (1 == m_mask_vector.at(i).mask_layerdata.count())
        {
                QStringList templist = m_mask_vector.at(i).mask_layerdata.at(0).layer_data.split(QRegExp("\\s+|\t"), QString::SkipEmptyParts);
                if (templist.count() > 1)
                {
                    QStringList list = templist.at(1).split("/");
                    if (list.count() > 1)
                    {
                        int layer_data = list.at(0).toInt();
                        int layer_type = list.at(1).toInt();
                        layout_data_string += QString("    %1 = mainlayout.read(%2, %3).merge()\n").arg(mask_name).arg(layer_data).arg(layer_type);
                    }
                }
        }
        else if (m_mask_vector.at(i).mask_layerdata.count() > 1)
        {
            for (int j = 0; j < m_mask_vector.at(i).mask_layerdata.count(); j ++)
            {
                QStringList templist = m_mask_vector.at(i).mask_layerdata.at(j).layer_data.split(QRegExp("\\s+|\t"), QString::SkipEmptyParts);
                QString  alias_string = m_mask_vector.at(i).mask_layerdata.at(j).alias;
                if (templist.count() > 1)
                {
                    QStringList list = templist.at(1).split("/");
                    if (list.count() > 1)
                    {
                        int layer_data = list.at(0).toInt();
                        int layer_type = list.at(1).toInt();
                        layout_data_string += QString("    %1 = mainlayout.read(%2, %3).merge()\n").arg(alias_string).arg(layer_data).arg(layer_type);
                    }
                }
            }
            layout_data_string += boolean;
        }
        gds_string += layout_data_string;
    }
    return gds_string;
}

QString RtsPythonWriter::get_gpu()
{
    if (m_use_gpu_or_cup == "gpu")
    {
        return "1";
    }
    else
    {
        return "0";
    }
}

}
