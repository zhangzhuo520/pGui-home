#include "model.h"
namespace ui {

static const int epsilon = 5;

SqlQueryModel::SqlQueryModel(QObject *parent)
{
    setParent(parent);
}

void SqlQueryModel::setquery(const QSqlQuery &query)
{
    setQuery(query);
}

QVariant SqlQueryModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
    {
        return QVariant();
    }

    QVariant value = QSqlQueryModel::data(item, role);

    if((Qt::DisplayRole == role))
    {
        if (value.toString().split('.').count() > 1 &&
                value.toString().split('.').at(1).left(5).toInt() >= epsilon)
        {
            return QString ::number(value.toDouble(), 'f', 4);
        }
        else
        {
            return value;
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (item.row() % 2)
        {
        return QBrush(QColor(Qt::lightGray));
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
    setParent(parent);
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
    case Qt::BackgroundColorRole :
        if (item.row() % 2)
        {
            return QColor(Qt::lightGray);
        }
        else
        {
            return QColor(Qt::white);
        }
    case Qt::SizeHintRole:
        return QSize(32, 16);
    default:
        return value;
    }
    return QVariant();
}

}
