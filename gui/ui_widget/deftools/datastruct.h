#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <QString>
#include <QStringList>
#include <QVector>
#include "render_layout_view.h"
namespace ui {


#ifndef MyDebug
#define MyDebug  qDebug()<<"[FILE:"<<__FILE__<<",LINE"<<__LINE__<<",FUNC"<<__FUNCTION__<<"]";
#endif

#ifndef NeedContinue
#define NeedContinue  qDebug()<<"[FILE:"<<__FILE__<<",LINE"<<__LINE__<<",FUNC"<<__FUNCTION__<<"] Donn't finished!";
#endif
/*
 * DataBase struct
 */
typedef struct defectsql
{
    QString table_id;
    QString orderBy;
    QString order;
    QString detdefgroup_id;
    QString limitIndex;
    QString pageCount;
}DefectSqlData;

typedef struct defectgroupsql
{
    QString table_id;
    QString orderBy;
    QString order;
    QString limitIndex;
    QString pageCount;
}defectGroupSqlData;

typedef struct Countgroupsql
{
    QString tableName;
    QString table_id;
}CountGroupSqlData;

typedef struct countdefectsql
{
    QString tableName;
    QString table_id;
    QString defGroup_id;
}CountDefectSqlData;

/*
 * Rts struct
 */
typedef struct rtslayerdata
{
   QString alias;
   QString layer_data;
   struct rtslayerdata& operator=(const struct rtslayerdata& data)
   {
       this->alias = data.alias;
       this->layer_data = data.layer_data;
       return *this;
   }

}RtsLayerData;

typedef struct rtsmaskdata
{
   QString mask_name;
   QString boolean;
   QVector <RtsLayerData> mask_layerdata;

   struct rtsmaskdata& operator=(const struct rtsmaskdata& data)
   {
       this->boolean = data.boolean;
       this->mask_name = data.mask_name;
       this->mask_layerdata.clear();
       for (int i = 0; i < mask_layerdata.count(); i ++)
       {
           mask_layerdata.append(mask_layerdata.at(i));
       }
       return *this;
   }
}RtsMaskData;

typedef struct rtssetupdata
{
   QString model_path;
   QVector <RtsMaskData> mask_table_data;
   QString mask_bias;
   QString delta_dose;
   QString delta_defocus;
   QString use_gpu_or_cup;
   QString binary_file;

   struct rtssetupdata& operator=(const struct rtssetupdata& data)
   {
       model_path = data.model_path;
       mask_bias = data.mask_bias;
       delta_dose = data.delta_dose;
       delta_defocus = data.delta_defocus;
       use_gpu_or_cup = data.use_gpu_or_cup;
       binary_file = data.binary_file;
       mask_table_data.clear();

       for (int i = 0; i < mask_table_data.count(); i ++)
       {
           mask_table_data.append(mask_table_data.at(i));
       }
       return *this;
   }
}RtsSetupData;

typedef struct rtsreviewinfo
{
    int index;
    QString range;
    QString x;
    QString y;
    QString output_path;

    struct rtsreviewinfo operator =(const struct rtsreviewinfo& data)
    {
        index = data.index;
        range = data.range;
        x = data.x;
        y = data.y;
        output_path = data.output_path;
        return *this;
    }
}RtsReviewInfo;

/*
 * Rts struct
 */
enum FileType
{
    job = 10,
    file
};

typedef struct fileinfo
{
    render::LayoutView* layout_view;
    FileType file_type;

    struct fileinfo operator = (const struct fileinfo & obj)
    {
        this->layout_view = obj.layout_view;
        this->file_type = obj.file_type;
        return *this;
    }
} FileInfo;

typedef struct frameinfo
{
    FileType file_type;
    QVector <FileInfo> fileinfo_vector;

    struct frameinfo operator=(const struct frameinfo& obj)
    {
        this->file_type = obj.file_type;
        this->fileinfo_vector.clear();
        for (int i = 0; i < obj.fileinfo_vector.count(); i ++)
        {
            fileinfo_vector.append(obj.fileinfo_vector.at(i));
        }

        return *this;
    }
} FrameInfo;

}

#endif // DATASTRUCT_H
