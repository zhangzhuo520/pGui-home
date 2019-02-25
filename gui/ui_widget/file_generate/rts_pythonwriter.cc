#include "rts_pythonwriter.h"

namespace ui {

RtsPythonWriter::RtsPythonWriter(const RtsSetupData *data)
{
    m_binary_file =  data->binary_file;
    m_delta_defocus = data->delta_defocus;
    m_delta_dose = data->delta_dose;
    m_mask_bias = data->mask_bias;
    m_use_gpu_or_cup = data->use_gpu_or_cup;

    for(int i = 0; i < data->mask_table_data.count(); i ++)
    {
        m_mask_name_list.append(data->mask_table_data[i].mask_name);

        for (int j = 0; j < data->mask_table_data[i].mask_layerdata.count(); j ++)
        {

        }
    }
}

void RtsPythonWriter::save_to_file()
{
    QString file_data =
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
            "    input_gds_1 = '/nfs/data/gds/model/PRD_layout.oas'\n"
            "    input_gds_2 = '/nfs/data/gds/model/PRD_layout.oas'\n"
  "\n"
            "    chip_boundary = [22016, 14773, 22036, 14793]  # this should be canvas coordinate\n"
            "    patch_size = 30\n"
  "\n"
            "    input_model_dir = '/nfs/data/model/mask_opt/ptd1.0_2dm_resist/result/result_0/'  # user selected model\n"
            "    prep_dir = './prepdb'\n"
  "\n"
            "    # set up the initialization flow based on utilty functions\n"
            "    conditions = get_conditions()\n"
            "    modeloptions = {}\n"
            "    psu.set_model(input_model_dir, conditions, modeloptions)\n"
  "\n"
            "    # Specify the input layout\n"
            "    psu.set_input_layout('mainlayout', input_gds_1, preprocess_setting={'dir': prep_dir, 'mode': 'auto'})\n"
            "    psu.set_input_layout('otherlayout', input_gds_2, preprocess_setting={'dir': prep_dir, 'mode': 'auto'})\n"
  "\n"
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
    QString rts_python_path = QDir::homePath() + "/.picasso_gui" + "/pgui_rts";
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

}
