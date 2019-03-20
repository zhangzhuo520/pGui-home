#ifndef UI_MEASURE_TABLE_H
#define UI_MEASURE_TABLE_H

#include <QTableView>
#include <QHeaderView>
#include <QProxyStyle>
#include <QDockWidget>
#include <QPainter>
#include <QSize>

#include "model/ui_measuretable_model.h"
#include "ui_measurepoint.h"
#include "deftools/defcontrols.h"

namespace ui {

class MeasureTable : public QWidget
{
    Q_OBJECT
public:
    explicit MeasureTable(QWidget *parent = 0);

    void init();

    virtual QSize sizeHint() const
    {
        return QSize(250, 60);
    }

    void only_clear_table_data();
    
signals:
    void signal_set_line_list(const QList <LineData>&);


public slots:
    void slot_context_menu(QPoint);

    void slot_delete_row();

    void slot_delete_all();

    void slot_update_index(QModelIndex);

    void slot_checked_line(QModelIndex);

    void slot_set_line_list(const QList <LineData>&);
private:
    TableView *m_table_view;
    MeasureTableModel *m_table_model;
    QList <LineData> m_linedata_list;
    MeasureTable* m_measure_table;
    int m_active_index;
};
}
#endif // UI_MEASURE_TABLE_H
