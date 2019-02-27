#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <QString>
#include <QStringList>
#include <QVector>
#define SYS_TIPBOX 1

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

#endif // DATASTRUCT_H
