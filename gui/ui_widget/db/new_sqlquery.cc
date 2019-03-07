#include "new_sqlquery.h"
namespace ui {
//defectSQL
#if 0
defectSQL::defectSQL()
{
    initData();
}

defectSQL::~defectSQL()
{
}

void defectSQL::initData()
{
    data.limitIndex = "";
    data.order = "";
    data.orderBy = "";
    data.pageCount = "";
    data.table_id = "";
    data.detdefgroup_id = "";
}

void defectSQL::setData(DefectSqlData newData)
{
    data.detdefgroup_id = newData.detdefgroup_id;
    data.limitIndex = newData.limitIndex;
    data.order = newData.order;
    data.orderBy = newData.orderBy;
    data.pageCount = newData.pageCount;
    data.table_id = newData.table_id;

    sqlstr = QString("select defect_id, size, x, y, category_id, patch_id"\
                     "from defect " \
                     "where check_id = %1 and category_id = %2 order by %3 %4 limit %5,%6;"\
                     ).arg(data.table_id).arg(data.detdefgroup_id).arg(data.orderBy).arg(data.order).arg(data.limitIndex).arg(data.pageCount);
}

bool defectSQL::isValidData()
{
    if(data.limitIndex == ""&&
            data.order == ""&&
            data.orderBy == ""&&
            data.pageCount == ""&&
            data.table_id == ""&&
            data.detdefgroup_id == "")
    {
        return false;
    }
    else
        return true;
}

QString defectSQL::outputSQL()
{
    if (isValidData())
    {
        return sqlstr;
    }
        else
            return NULL;
}

//defectGroupSQL
defectGroupSQL::defectGroupSQL()
{
    initData();
}

defectGroupSQL::~defectGroupSQL()
{
}

void defectGroupSQL::initData()
{
    data.limitIndex = "";
    data.order = "";
    data.orderBy = "";
    data.pageCount = "";
    data.table_id = "";
}

void defectGroupSQL::setData(defectGroupSqlData newData)
{
    data.limitIndex = newData.limitIndex;
    data.order = newData.order;
    data.orderBy = newData.orderBy;
    data.pageCount = newData.pageCount;
    data.table_id = newData.table_id;

    sqlString = QString("select check_id, category_id, defect_count, worst_size "\
                        "from category " \
                        "where check_id = %1 order by %2 %3 limit %4 , %5;"\
                        ).arg(data.table_id).arg(data.orderBy).arg(data.order).arg(data.limitIndex).arg(data.pageCount);
}

bool defectGroupSQL::isValidData()
{
    if(data.limitIndex == ""&&
            data.order == ""&&
            data.orderBy == ""&&
            data.pageCount == ""&&
            data.table_id == "")
    {
        return false;
    }
    else
        return true;
}

QString defectGroupSQL::getSqlStr()
{
    return sqlString;
}

QSqlQuery defectGroupSQL::outputSQL()
{
    if (isValidData())
    {
        return QSqlQuery(sqlString);
    }
    else
        return NULL;
}

QSqlQuery defectGroupSQL::limitSQL(int offset)
{
        if (isValidData())
        {
            return QSqlQuery(QString("select check_id, category_id, defect_count, worst_size "\
                                     "from category "\
                                     "where check_id = %1 order by %2 %3 limit %4 %5;").arg(data.table_id).arg(data.orderBy).arg(data.order).arg(data.limitIndex).arg(QString::number(offset)));
        }
        else
            return NULL;
}

//CountGroupSQL
CountGroupSQL::CountGroupSQL()
{
    initData();
}

CountGroupSQL::~CountGroupSQL()
{
}

void CountGroupSQL::initData()
{
    data.tableName = "";
    data.table_id = "";
}

void CountGroupSQL::setData(CountGroupSqlData newData)
{
    data.tableName = newData.tableName;
    data.table_id = newData.table_id;
}

bool CountGroupSQL::isValidData()
{
    if (data.tableName == ""&&
            data.table_id == "")
    {
        return false;
    }
    else
    {
        return true;
    }
}

QSqlQuery CountGroupSQL::outputSQL()
{
    if (isValidData())
    {
        return QSqlQuery(QString("select count(*) from  %1 where check_id = %2;")\
                         .arg(data.tableName).arg(data.table_id));
    }
    else
        return NULL;
}

//CountDefectSQL
CountDefectSQL::CountDefectSQL()
{
    initData();
}

CountDefectSQL::~CountDefectSQL()
{
}

void CountDefectSQL::initData()
{
    data.defGroup_id = "";
    data.tableName = "";
    data.table_id = "";
}

void CountDefectSQL::setData(CountDefectSqlData newData)
{
    data.defGroup_id = newData.defGroup_id;
    data.tableName = newData.tableName;
    data.table_id = newData.table_id;
}

bool CountDefectSQL::isValidData()
{
    if (data.defGroup_id == "" &&
            data.tableName == ""&&
            data.table_id == "")
    {
        return false;
    }
    else
    {
        return true;
    }
}

QSqlQuery CountDefectSQL::outputSQL()
{
    if (isValidData())
    {
        return QSqlQuery(QString("select count(*) from %1 where check_id = %2 and category_id = %3;"\
                                 ).arg(data.tableName).arg(data.table_id).arg(data.defGroup_id));
    }
    else
        return NULL;
}
#endif
}
