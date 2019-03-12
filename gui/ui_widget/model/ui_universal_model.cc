#include "ui_universal_model.h"

namespace ui {

UniversalModel::UniversalModel(QObject *parent)
{
}

UniversalModel::~UniversalModel()
{
}

void UniversalModel::setHeaderList(const QStringList &headerlist)
{
    if (headerlist.count() < 1)
    {
        logger_console.debug("Header data Error !");
        return;
    }
    m_header_list = headerlist;
    reset();
}

void UniversalModel::delete_line(const int & index)
{
    if (index > -1 && index < m_data_list.count())
    {
        m_data_list.removeAt(index);
        reset();
    }
}

void UniversalModel::set_data_list(const QStringList & DataList)
{
    if (DataList.count() < 0)
    {
        logger_console.debug("table data Error !");
        return;
    }

    m_data_list = DataList;
    reset();
}

void UniversalModel::clear_data_list()
{
    m_data_list.clear();
    reset();
}

QModelIndex UniversalModel::parent(const QModelIndex &child)const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QModelIndex UniversalModel::index(int row, int column, const QModelIndex &parent) const
{
       return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex();
}

int UniversalModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data_list.count();
}

int UniversalModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_header_list.count();
}

QVariant UniversalModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
    {
        return QVariant();
    }
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;

    if((Qt::DisplayRole == role))
    {
        QStringList list = m_data_list.at(item.row()).split("##", QString::SkipEmptyParts);
        for (int i = 0; i < list.count(); i ++)
        {
            if (i == item.column())
                return list.at(i);
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
    {
        return QVariant();
    }

    return QVariant();
}

QVariant UniversalModel::headerData(int section, Qt::Orientation orientation, int role)const
{
    if(role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal)
    {
        for (int i = 0; i < m_header_list.count(); i ++)
        {
            if (section == i)
                return m_header_list.at(i);
        }
    }
    return QVariant();
}
}
