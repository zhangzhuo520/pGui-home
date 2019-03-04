#ifndef FileProjectWidget_H
#define FileProjectWidget_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QLayout>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QToolTip>
#include <QCursor>

#include "deftools/global.h"
#include "deftools/cmessagebox.h"
#include "model/ui_fileproject_model.h"
#include "deftools/defcontrols.h"

namespace ui{
class FileProjectWidget : public QWidget
{
    Q_OBJECT
public:
    typedef FileProjectModel::layout_view_iter layout_view_iter;
    explicit FileProjectWidget(QWidget *parent = 0);

    void init();

    layout_view_iter get_layout_view_iter(int index)
    {
        return m_project_tablemodel->get_layout_view_iter(index);
    }    
    bool is_file_exist(QString);

    void delete_file(QString);

    int file_count() const
    {
        return m_project_tablemodel->rowCount(QModelIndex());
    }
signals:
    void signal_close_currentFile(QString);
    void signal_openFile();
    void signal_create_canvas(QModelIndex);
    void signal_create_overlay_canvas(QModelIndex);
    void signal_close_tab(QString);
    void signal_append_file(int);

public slots:
    void slot_DoubleClickItem(QModelIndex);
    void slot_ClickItem(QModelIndex);
    void slot_context_menu(QPoint);
    void slot_addFile(QString, bool);
    void slot_CloseItem();
    void slot_OpenFile();
    void slot_AppendFile();
    void slot_showToolTip(QModelIndex);

private:
    QString m_active_filename;
    int m_active_index;
    TableView *m_project_table;
    FileProjectModel *m_project_tablemodel;

};
}
#endif // FileProjectWidget_H
