#include "model.h"

SqlQueryModel::SqlQueryModel(QObject *parent)
{

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
#endif
