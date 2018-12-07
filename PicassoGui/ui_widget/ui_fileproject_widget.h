#ifndef FileProjectWidget_H
#define FileProjectWidget_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QLayout>
#include <QMenu>
#include <QAction>
#include "deftools/global.h"
#include "deftools/cmessagebox.h"
#include "ui_fileproject_model.h"

namespace ui{
class FileProjectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileProjectWidget(QWidget *parent = 0);

    void init();

    std::vector<render::LayoutView>::iterator get_layout_view_iter(int index)
    {
        return m_project_tablemodel->get_layout_view_iter(index);
    }    

signals:
    void signal_DoubleClickItem(QModelIndex);
    void close_currentFile(int);
    void signal_openFile();

public slots:
    void slot_DoubleClickItem(QModelIndex);
    void slot_ClickItem(QModelIndex);
    void slot_context_menu(QPoint);
    void slot_addFile(QString);
    void slot_CloseItem();
    void slot_Activated(QModelIndex);
    void slot_OpenFile();

private:

    int m_active_index;
    QTableView *m_project_table;
    FileProjectModel *m_project_tablemodel;
};
}
#endif // FileProjectWidget_H