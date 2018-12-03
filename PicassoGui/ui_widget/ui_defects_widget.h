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

namespace ui {
class DefectsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefectsWidget(QWidget *parent = 0, QString m_db_path = "", QModelIndex* modelIndex = 0, int m_jobindex = 0);

    void initm_defects_table();

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

    void slot_m_descent_buttonCheck();

    void slot_m_ascen_buttonCheck();

    void slot_pervPage();

    void slot_nextPage();
private:
    QString m_db_path;

    QTableView *m_defects_table;

    QWidget *m_button_bar;

    QLabel *m_sort_lable;

    PushButton *m_perv_button;

    PushButton *m_next_button;

    QModelIndex *m_model_index;

    qlonglong m_total_count;

    Commbox *m_sort_commbox;

    QRadioButton *m_descent_button;

    QRadioButton *m_ascen_button;

    PushButton *m_extract_button;

    PushButton *m_moreoptions_button;

    SqlQueryModel *m_defects_model;

    QVBoxLayout *m_vlayout;

    QHBoxLayout *m_hlayout;

    QHBoxLayout *m_hlayout_s;

    defectSQL *m_defects_query;

    CountDefectSQL *m_count_defects_query;

    DefectSqlData m_defect_data;

    CountDefectSqlData m_countdefect_data;

    SQLManager *m_sqlmanager;

    int m_jobindex;

    int m_current_page;

    int m_table_id;

    int m_group_id;

    QStringList m_header_list;
};
}
#endif // DefectsWidget_H
