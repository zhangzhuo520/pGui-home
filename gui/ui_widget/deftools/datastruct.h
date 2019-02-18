#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <QString>

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
}RtsLayerData;

typedef struct rtsmaskdata
{
   QString mask_name;
   QVector <RtsLayerData> mask_layerdata;
}RtsMaskData;

typedef struct rtssetupdata
{
   QString gds_path;
   QString model_path;
   QVector <RtsMaskData> mask_table_data;
   QString mask_bias;
   QString delta_dose;
   QString delta_defocus;
   QString use_gpu_or_cup;
   QString binary_file;
}RtsSetupData;

#endif // DATASTRUCT_H
