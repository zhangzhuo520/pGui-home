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
#include "model/ui_fileproject_model.h"
#include "deftools/defcontrols.h"

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
    bool is_file_exist(QString);

    void delete_file(QString);
signals:
    void signal_close_currentFile(QString);
    void signal_openFile();
    void signal_creat_canves(QModelIndex);
    void signal_creat_overlay_canves(QModelIndex);

public slots:
    void slot_DoubleClickItem(QModelIndex);
    void slot_ClickItem(QModelIndex);
    void slot_context_menu(QPoint);
    void slot_addFile(QString, bool);
    void slot_CloseItem();
    void slot_OpenFile();

//protected:
//    virtual void

private:
    QString m_active_filename;
    int m_active_index;
    TableView *m_project_table;
    FileProjectModel *m_project_tablemodel;
};
}
#endif // FileProjectWidget_H
