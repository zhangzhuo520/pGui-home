#ifndef UI_MEASURETABLE_MODEL_H
#define UI_MEASURETABLE_MODEL_H
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QList>
#include <QBrush>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QPushButton>

#include "../ui_measurepoint.h"

namespace ui {

class MeasureTableModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit MeasureTableModel(QObject *parent = 0);
    ~MeasureTableModel();

    void setHeaderList(const QStringList &);
    void delete_line(const int &);
    void set_line_list(const QList <LineData*> &);
    virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole)const;
private:
   QList <LineData*> m_linedata_list;
   QStringList m_header_list;
};
}
#endif // UI_MEASURETABLE_MODEL_H
