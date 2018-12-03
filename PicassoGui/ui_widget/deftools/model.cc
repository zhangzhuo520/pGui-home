#include "model.h"
namespace ui {
SqlQueryModel::SqlQueryModel(QObject *parent)
{
    Q_UNUSED(parent);
}

void SqlQueryModel::setquery(const QSqlQuery &query)
{
    setQuery(query);
}
#if 1
QVariant SqlQueryModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
    {
        return QVariant();
    }
    QVariant value = QSqlQueryModel::data(item, role);
    if((Qt::DisplayRole == role))
    {
        return value;
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (item.row() % 2)
        {
            return QColor(Qt::lightGray);
        }
        else
        {
            return QColor(Qt::white);
        }
    }
    else
        return value;
}

TreeModel::TreeModel(QObject *parent)
{
    Q_UNUSED(parent);
}

QVariant TreeModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
    {
        return QVariant();
    }
    QVariant value = QStandardItemModel::data(item, role);
    switch (role) {
    case Qt::DisplayRole:
        return value;
    case Qt::TextAlignmentRole:
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    case Qt::SizeHintRole:
        return QSize(32, 24);
    default:
        return value;
    }
    return QVariant();
}


//QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
//{
//    TreeItem *parentItem = NULL;
//    if (!parent.isValid())
//    {
//         return QModelIndex();
//    }
//    else
//    {
//         parentItem = static_cast <TreeItem *> parent.internalPointer();
//    }

//    if (row < 0 || column < 0 || row >= parentItem->rowCount()
//            || row >= parentItem->columnCount())
//    {
//        return QModelIndex();
//    }
//    else
//    {
//        createIndex(row, column, parentItem);
//    }
//}

//QModelIndex TreeModel::parent(const QModelIndex &child)
//{
//    return child.parent();
//}
}
#endif
