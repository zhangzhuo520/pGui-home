#ifndef DefectsWidget_H
#define DefectsWidget_H

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
#include <QtSql/QSqlQueryModel>
#include <QToolBar>
#include <QSizePolicy>
#include "deftools/model.h"

class DefectsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefectsWidget(QWidget *parent = 0, QString DbPath = "", QModelIndex* modelIndex = 0, int jobIndex = 0);

    void initdefectsTable();

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

     QTableView * getTableView();
protected:
    void closeEvent(QCloseEvent *e)
    {
        Q_UNUSED(e);
    }

signals:
    void signal_showDefects(QModelIndex *);

public slots:
    void slot_showDefects(QModelIndex *);

    void slot_defectsUpdata(QModelIndex *);

    void slot_changSortQrder(QString);

    void slot_descentButtonCheck();

    void slot_ascentButtonCheck();

    void slot_pervPage();

    void slot_nextPage();
private:
    QString DbPath;

    QTableView *defectsTable;

    QWidget *Buttonbar;

    QLabel *sortLable;

    PushButton *pervButton;

    PushButton *nextButton;

    QModelIndex *index;

    qlonglong totalCount;

    Commbox *sortCombox;

    QRadioButton *descentButton;

    QRadioButton *ascentButton;

    PushButton *ExtractButton;

    PushButton *MoreOptionsButton;

    SqlQueryModel *defectsModel;

    QVBoxLayout *Vlayout;

    QHBoxLayout *Hlayout;

    QHBoxLayout *Hlayout1;

    defectSQL *DefectsQuery;

    CountDefectSQL *CountDefectsQuery;

    DefectSqlData DefectsData;

    CountDefectSqlData CountDefectData;

    SQLManager *sqlManager;

    int jobIndex;

    int CurrentPage;

    int tableId;

    int groupId;

    QStringList headerList;
    
};

#endif // DefectsWidget_H
