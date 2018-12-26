#ifndef DefGroup_H
#define DefGroup_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QLayout>
#include <QLabel>
#include <QtSql/QSqlQueryModel>
#include <QToolBar>
#include <QRadioButton>

#include "deftools/defcontrols.h"
#include "db/sqlquery.h"
#include "db/sqlmanager.h"
#include "model/ui_sqltabel_model.h"


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

//    void changeSortKey();

    void resizeColumns();

    void sortTable();

    void setData();

    void setTotal();

    void update_page();

    void showDefects(QModelIndex *);

    void updata_all_data(QModelIndex *);

protected:
    void closeEvent(QCloseEvent *e)
    {
        Q_UNUSED(e);
    }

signals:
    void signal_showDefects(QModelIndex, int);
    
public slots:
    void slot_showDefects(QModelIndex);

//    void slot_DefGroupUpdata(QModelIndex *);

//    void slot_changSortQrder(QString);

//    void slot_descentButtonCheck();

//    void slot_ascentButtonCheck();

    void slot_pervPage();

    void slot_nextPage();

    void slot_jump_page(QString);

    void slot_sort_by_column(int, Qt::SortOrder);
private:
    void update_page_number();

    void jump_page(int);

    QString DbPath;

    QTableView *DefGroupTable;

    QWidget *Buttonbar;

    QLabel *sortLable;

    PageJumpEdit * m_page_jump_edit;

    QPushButton *pervButton;

    QPushButton *nextButton;

    QModelIndex *index;

    qlonglong totalCount;

    Commbox *sortCombox;

    QRadioButton *descentButton;

    QRadioButton *ascentButton;

    SqlQueryModel *DefGroupModel;

    QVBoxLayout *m_vlayout;

    QHBoxLayout *Hlayout;

    QHBoxLayout *m_hlayout1;

    defectGroupSQL *DefectGroupQuery;

    CountGroupSQL *CountGroupQuery;

    defectGroupSqlData DefectGroupData;

    CountGroupSqlData CountGroupData;

    QLabel *m_pagecount_label;

    SQLManager *sqlManager;

    int jobIndex;

    int CurrentPage;

    int m_current_number;

    int tableId;

    int groupId;

    QStringList headerList;
};
}
#endif // DefGroup_H
