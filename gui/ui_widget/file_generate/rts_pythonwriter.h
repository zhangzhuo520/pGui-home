#ifndef RTS_PYTHONWRITER_H
#define RTS_PYTHONWRITER_H
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QDebug>


#include "../deftools/datastruct.h"
#include "../qt_logger/pgui_log_global.h"

namespace ui {

class RtsPythonWriter
{
public:
    RtsPythonWriter(const RtsSetupData *data = 0);

    void save_to_file();

    void set_canvas_pos(const double&, const double&, const double&, const double&);
private:
    void create_pframe_file();

    void create_run_file();

    int m_canvas_letf;
    int m_canvas_right;
    int m_canvas_top;
    int m_canvas_bottom;
    int m_pitch_size;

    QString text_string();
    QString input_gds();
    QString input_layout();
    QString gds_layout();

    QString get_gpu();

    QString m_binary_file;
    QString m_delta_defocus;
    QString m_delta_dose;
    QString m_mask_bias;
    QString m_use_gpu_or_cup;
    QString m_model_path;

    QStringList m_mask_name_list;
    QStringList m_layer_data_type_list;
    QStringList m_alias_list;
    QStringList valid_gds_list;

    QString m_gds_temp_string;
    QString m_chip_boundary_temp_string;
    QString m_patch_size_temp_string;
    QString m_prep_dir;

    QVector <RtsMaskData> m_mask_vector;
};
}

#endif // RTS_PYTHONWRITER_H
