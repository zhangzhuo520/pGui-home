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

namespace ui {

class CheckList : public QWidget
{
    Q_OBJECT
    
public:
    explicit CheckList(int width, int height, QWidget *parent = 0);

    void initToolbar();

    void initFoundWidget();

    void initTreeView();

    void updataTreeView();

    void readDB(QString DBname);

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

private slots:
    void slot_CheckListContextMenu(const QPoint&);

    void slot_showDefGroup(QModelIndex);

    void slot_openLVCK(QString);

    void slot_BtnRename();

    void slot_BtnDelete();

    void slot_BtnLoad();

    void slot_BtnSave();

    void slot_ExportCSV();

    void slot_BtnFound(bool);

    void slot_RenameOk();
public slots:
    void slot_readDB(QString);

private:
    Commbox *m_checklist_commbox;
    PushButton *m_rename_button;
    PushButton *m_delete_button;
    PushButton *m_load_button;
    MenuButton *m_save_button;
    PushButton *m_prev_button;
    PushButton *m_next_button;
    PushButton *m_found_button;

    PushButton *m_rtsconfig_button;
    PushButton *m_runrts_button;
    PushButton *m_closejob_button;
    PushButton *m_appendjob_button;
    PushButton *m_setting_button;

    QDialog *m_rename_dialog;
    QLineEdit *m_rename_edit;

    Toolbar *m_checklist_toolbar;
    QTreeView *m_checklist_tree;
    QStandardItemModel *m_checklist_model;
    SQLManager *m_sqlmanager;
    QStringList m_joblist;
    QString m_jobdata;
    QString m_count;
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
};
}
#endif // CheckList_H
