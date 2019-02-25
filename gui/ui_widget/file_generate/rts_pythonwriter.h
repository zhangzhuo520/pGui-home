#ifndef RTS_PYTHONWRITER_H
#define RTS_PYTHONWRITER_H
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDir>
#include <QFile>
#include <QDebug>
#include "../deftools/datastruct.h"

namespace ui {

class RtsPythonWriter
{
public:
    RtsPythonWriter(const RtsSetupData *data = 0);

    void save_to_file();
private:
    QString m_binary_file;
    QString m_delta_defocus;
    QString m_delta_dose;
    QString m_mask_bias;
    QString m_use_gpu_or_cup;
    QStringList m_mask_name_list;
    QStringList m_layer_data_type_list;
    QStringList m_alias_list;
    QMap <QString, QStringList> m_mask_tab_map;

    QString m_gds_temp_string;
    QString m_chip_boundary_temp_string;
    QString m_patch_size_temp_string;
    QString m_input_model_dir;
    QString m_prep_dir;

};
}

#endif // RTS_PYTHONWRITER_H
