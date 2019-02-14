#include "ui_rtsmask_model.h"

RtsMaskModel::RtsMaskModel(QObject *parent)
{
    m_alias_list << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h"
                 << "i" << "j" << "k" << "l" << "m" << "n" << "o" << "p"
                 << "q" << "r" << "s" << "t" << "u" << "v" << "w" << "x"
                 << "y" << "z";
    m_header_list << "Alias" << "GDS : Layer/DT";
}

RtsMaskModel::~RtsMaskModel()
{
    m_alias_list.clear();
    m_header_list.clear();
}

void RtsMaskModel::delete_row(const int & row)
{
    m_data_list.removeAt(row);
    endRemoveColumns();
}

void RtsMaskModel::delete_all()
{
    m_data_list.clear();
    endRemoveColumns();
}

void RtsMaskModel::set_data_list(const QStringList & list)
{
    m_data_list = list;
    endResetModel();
}

void RtsMaskModel::append_row(const QString & String)
{
    m_data_list.append(String);
    endResetModel();
}

QModelIndex RtsMaskModel::parent(const QModelIndex &child)const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QModelIndex RtsMaskModel::index(int row, int column, const QModelIndex &parent) const
{
       return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex();
}

int RtsMaskModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data_list.count();
}

int RtsMaskModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_header_list.count();
}

QVariant RtsMaskModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
    {
        return QVariant();
    }
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;

    if((Qt::DisplayRole == role))
    {
        switch (item.column()) {
        case 0:
        {
            return m_alias_list.at(item.row());
        }
        case 1:
        {
            return m_data_list.at(item.row());
        }
        default:
            break;
        }
    }
    else
    {
        return QVariant();
    }

    return QVariant();
}

QVariant RtsMaskModel::headerData(int section, Qt::Orientation orientation, int role)const
{
    if(role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal)
    {
        switch (section) {
        case 0:
            return m_header_list.at(0);
        case 1:
            return m_header_list.at(1);
        default:
            break;
        }
    }
    else if(orientation == Qt::Horizontal)
    return QVariant();
}


