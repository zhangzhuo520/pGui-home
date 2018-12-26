#ifndef UI_MEASURE_TABLE_H
#define UI_MEASURE_TABLE_H

#include <QTableView>
#include <QHeaderView>
#include <QProxyStyle>
#include <QDockWidget>
#include <QPainter>

#include "ui_measuretable_model.h"
#include "ui_measurepoint.h"
#include "deftools/defcontrols.h"

namespace ui {

class MeasureTable : public DockWidget
{
    Q_OBJECT
public:
    explicit MeasureTable(QWidget *parent = 0);

    void init();

    void set_line_list(QList <LineData>);
    
signals:
    
public slots:

private:
    QTableView *m_table_view;
    MeasureTableModel *m_table_model;
    QList <LineData> m_linedata_list;
    MeasureTable* m_measure_table;
};
}
#endif // UI_MEASURE_TABLE_H
