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

typedef struct countgroupsql
{
    QString tableName;
    QString table_id;
}CountGroupSqlData;

typedef struct CountDefectSql
{
    QString tableName;
    QString table_id;
    QString defGroup_id;
}CountDefectSqlData;

typedef struct fileinfo
{
    int id;
    QString filename;
}FileInfo;

#endif // DATASTRUCT_H
