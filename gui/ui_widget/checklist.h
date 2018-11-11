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
#include "deftools/defcontrols.h"
#include "defgroup.h"
#include "db/sqlmanager.h"
#include "deftools/framelesshelper.h"
#include <QSqlQuery>

class QSizePolicy;
class CheckList : public QWidget
{
    Q_OBJECT
    
public:
    explicit CheckList(QWidget *parent = 0);

    void initToolbar();

    void initTreeView();

    void updataTreeView();

    void readDB(QString DBname);

    ~CheckList();
protected:

signals:
    void signal_showDefGroup(QModelIndex, int);

private slots:
    void slot_CheckListContextMenu(const QPoint&);

    void slot_showDefGroup(QModelIndex);

public slots:
    void slot_readDB(QString);

private:
    Commbox *CheckListCommbox;
    PushButton *btn_rename;
    PushButton *btn_delete;
    PushButton *btn_load;
    PushButton *btn_save;
    PushButton *btn_prev;
    PushButton *btn_next;

    int jobIndex;
    QToolBar *CheckListToolbar;
    QTreeView *CheckListTree;
    QStandardItemModel *CheckListModel;
    SQLManager *sqlmanager;
    QStringList jobList;
    QString jobData;
    QString Count;
    QStringList headerList;

    QMap < QString , QString> maskMap;
    QVector <QMap < QString , QString> >maskVector;

    QMap < QString , QString> pw_conditionMap;
    QVector <QMap < QString , QString> >pw_conditionVector;

    QMap < QString , QString> detectorMap;
    QVector <QMap < QString , QString> >detectorVector;
};

#endif // CheckList_H
