#ifndef UI_DATA_TABLE_H
#define UI_DATA_TABLE_H
#include <QTableView>
#include <QHeaderView>
#include <QPoint>
#include <QMenu>
#include "../model/ui_universal_model.h"
namespace ui {
class DataTable : public QTableView
{
    Q_OBJECT
public:
    explicit DataTable(QWidget *parent, const QStringList & HeadList = QStringList());
    ~DataTable();

    void append_data(const QString &);
    void delete_all_data();
    void delete_data(const int&);

    int count() const;

public slots:
    void slot_context_menu(QPoint);
    void slot_delete_row();
    void slot_delete_all();
    void slot_update_index(QModelIndex);

protected:
    virtual void currentChanged(const QModelIndex &current,
                                const QModelIndex &previous)
    {
        emit clicked(current);
    }


private:
    UniversalModel *m_data_model;
    QStringList m_tabel_data_list;
    int m_active_index;
};
}
#endif // UI_DATA_TABLE_H
