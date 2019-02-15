#ifndef UI_GAUGETABLE_H
#define UI_GAUGETABLE_H

#include <QTableView>
#include <QHeaderView>
#include <QLayout>
#include "model/ui_gauge_model.h"
namespace ui {

class GaugeTable : public QTableView
{
    Q_OBJECT
public:
    explicit GaugeTable(QWidget *parent = 0);
    void set_header_list(QString);
    void set_data_list(QStringList);
    
signals:
    void signal_send_gauge_data(QModelIndex);

public slots:

    void slot_send_data(QModelIndex);

private:
    void init_tableview();
    QTableView *m_table_view;
    GaugeModel *m_table_model;
    
};

}

#endif // UI_GUAGETABLE_H
