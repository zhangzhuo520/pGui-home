#ifndef FileProjectWidget_H
#define FileProjectWidget_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QLayout>
#include <QDebug>
class FileProjectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileProjectWidget(QWidget *parent = 0);

    void init();
    
signals:
    void signal_click_fileItem(QModelIndex);

public slots:
    void slot_click_fileItem(QModelIndex);

    void slot_addFile(QString);
private:
    QTableView *projectTable;

    QStandardItemModel *projectTableModel;
};

#endif // FileProjectWidget_H
