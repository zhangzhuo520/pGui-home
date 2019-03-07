#ifndef NEW_SQLQUERY_H
#define NEW_SQLQUERY_H

#include <QObject>
#include <QtSql/QSqlQuery>
#include <stdio.h>
#include <QDebug>
#include "../deftools/datastruct.h"
#include "../deftools/global.h"
namespace ui {
#if 0
class defectSQL
{
public:
    defectSQL();
    ~defectSQL();

    void initData();
    void setData(DefectSqlData);
    bool isValidData();

    QString outputSQL();

private:
    QString sqlstr;

    DefectSqlData data;
};

class defectGroupSQL
{
public:
    defectGroupSQL();

    ~defectGroupSQL();

    void initData();
    void setData(defectGroupSqlData);
    bool isValidData();

    QString getSqlStr();

    QSqlQuery outputSQL();

    QSqlQuery limitSQL(int offset);

private:
    defectGroupSqlData data;

    QString sqlString;
};

class CountGroupSQL
{
public:
    CountGroupSQL();
    ~CountGroupSQL();

    void initData();
    void setData(CountGroupSqlData);
    bool isValidData();

    QSqlQuery outputSQL();

private:
    CountGroupSqlData data;
};

class CountDefectSQL
{
public:
    CountDefectSQL();
    ~CountDefectSQL();

    void initData();
    void setData(CountDefectSqlData);
    bool isValidData();

    QSqlQuery outputSQL();

private:
    CountDefectSqlData data;
};
#endif
}

#endif // SQLQUERY_H
