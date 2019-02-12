#ifndef UI_DATA_TABLE_H
#define UI_DATA_TABLE_H

#include <QTableView>
#include "../model/ui_measuretable_model.h"

class DataTable : public QTableView
{
    Q_OBJECT
public:
    explicit DataTable(QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // UI_DATA_TABLE_H
