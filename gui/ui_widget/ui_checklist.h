#ifndef CheckList_H
#define CheckList_H

#include <QWidget>
#include <QTreeView>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStandardItemModel>
#include <QToolBar>
#include <QVector>
#include <QMouseEvent>
#include <QDockWidget>
#include <QSqlQuery>
#include <QSizePolicy>
#include <QLayout>

#include "ui_defgroup.h"
#include "db/sqlmanager.h"
#include "deftools/framelesshelper.h"
#include "deftools/defcontrols.h"
#include "deftools/global.h"
#include "model/ui_checklist_model.h"
#include "deftools/cmessagebox.h"

namespace ui {
class CheckList : public QWidget
{
    Q_OBJECT
    typedef struct jobkey
    {
        int key;
        bool isUse;
    } JobKey;

public:
    explicit CheckList(int width, int height, QWidget *parent = 0);

    void initJobKey();

    void initTreeView();

    void updataTreeView();

    void new_update_treeview();

    void read_database(const QString&);

    void new_read_database(const QString&);

    int get_job_key();

    void close_job_key(int);

    void set_read_new_database(const bool&);

    ~CheckList();

    QSize sizeHint() const
    {
        return QSize(m_width, m_height);
    }

    void set_width(int width)
    {
        m_width = width;
    }

    void set_height(int height)
    {
        m_height = height;
    }

protected:

signals:
    void signal_showDefGroup(QModelIndex, int);
    void signal_close_job(QString);
    void signal_append_job(QString);
    void signal_close_database_widget(int);
    void signal_coverage_job();

private slots:    
    void slot_close_currentjob();
    void slot_CheckListContextMenu(const QPoint&);
    void slot_update_current_index(QModelIndex);
    void slot_showDefGroup(QModelIndex);
    void slot_coverage_job();

public slots:
    void slot_add_job(QString);
    void slot_close_job(QString);

private:
    bool m_is_read_new_database;

    void remove_job(int);
    QModelIndex get_current_rootindex(QModelIndex);

    TreeView *m_checklist_tree;
    TreeModel *m_checklist_model;
    SQLManager *m_sqlmanager;

    QString m_jobdata;
    QString m_count;
    QString m_cond_count;
    QStringList m_headerlist;

    QMap < QString , QString> m_maskmap;
    QVector <QMap < QString , QString> >m_maskvector;

    QMap < QString , QString> m_pw_conditionmap;
    QVector <QMap < QString , QString> >m_pw_conditionvector;

    QMap < QString , QString> m_detectormap;
    QVector <QMap < QString , QString> >m_detectorvector;
    int m_jobindex;

    QWidget *m_findwidget;
    QLineEdit *m_findedit;
    PushButton *m_infound_button;
    QVBoxLayout *m_vlayout;
    QHBoxLayout *m_hlayout;

    int m_width;
    int m_height;

    int m_active_tree_index;
    QString m_active_index_name;

    QStringList m_jobpath_list;
    QVector <JobKey> m_jobkey_vector;

    QVector <rootFileItem *> m_rootitem_vector;
};
}
#endif // CheckList_H
