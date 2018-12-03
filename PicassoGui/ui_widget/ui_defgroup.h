#ifndef DefGroup_H
#define DefGroup_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QLayout>
#include <QLabel>
#include <QRadioButton>
#include "deftools/defcontrols.h"
#include "db/sqlquery.h"
#include "db/sqlmanager.h"
#include "deftools/model.h"
#include <QtSql/QSqlQueryModel>
#include <QToolBar>

namespace ui {
class DefGroup : public QWidget
{
    Q_OBJECT
public:
    explicit DefGroup(QWidget *parent = 0, QString DbPath = "", QModelIndex* modelIndex = 0, int jobIndex = 0);
    ~DefGroup(){}

    void initDefGroupTable();

    void initOtherButton();

    void addLayout();

    void initSql();

    void updataTable();

    void openDB();

    void changeSortKey();

    void resizeColumns();

    void sortTable();

    void setData();

    void setTotal();

    void upDataPage();

    void showDefects(QModelIndex *);

protected:
    void closeEvent(QCloseEvent *e)
    {
        Q_UNUSED(e);
    }

signals:
    void signal_showDefects(QModelIndex, int);
    
public slots:
    void slot_showDefects(QModelIndex);

    void slot_DefGroupUpdata(QModelIndex *);

    void slot_changSortQrder(QString);

    void slot_descentButtonCheck();

    void slot_ascentButtonCheck();

    void slot_pervPage();

    void slot_nextPage();
private:
    QString DbPath;

    QTableView *DefGroupTable;

    QWidget *Buttonbar;

    QLabel *sortLable;

    PushButton *pervButton;

    PushButton *nextButton;

    QModelIndex *index;

    qlonglong totalCount;

    Commbox *sortCombox;

    QRadioButton *descentButton;

    QRadioButton *ascentButton;

    SqlQueryModel *DefGroupModel;

    QVBoxLayout *Vlayout;

    QHBoxLayout *Hlayout;

    QHBoxLayout *Hlayout1;

    defectGroupSQL *DefectGroupQuery;

    CountGroupSQL *CountGroupQuery;

    defectGroupSqlData DefectGroupData;

    CountGroupSqlData CountGroupData;

    SQLManager *sqlManager;

    int jobIndex;

    int CurrentPage;

    int tableId;

    int groupId;

    QStringList headerList;
};
}
#endif // DefGroup_H
