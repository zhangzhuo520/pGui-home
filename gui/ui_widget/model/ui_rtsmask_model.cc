#include "ui_rtsmask_model.h"

namespace ui {
RtsMaskModel::RtsMaskModel(QObject *parent):
    QAbstractItemModel(parent)
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

void RtsMaskModel::delete_row()
{
    m_data_list.removeAt(m_data_list.count() - 1);
    reset();
}

void RtsMaskModel::delete_all()
{
    m_data_list.clear();
    reset();
}

const QStringList &RtsMaskModel::get_alias_list()
{
    return m_alias_list;
}

const QStringList &RtsMaskModel::get_layerdata_list()
{
    return m_data_list;
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
    else if (Qt::ToolTipRole == role)
    {
        if (item.column() == 1)
        {
            return m_data_list.at(item.row());
        }
    }
    else
    {
        return QVariant();
    }

    return QVariant();
}

bool RtsMaskModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;

    if((Qt::DisplayRole == role))
    {
        if (index.column() == 1)
        {
            m_data_list[index.row()] = value.toString();
            return true;
        }
    }
    return true;
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

    return QVariant();
}

Qt::ItemFlags RtsMaskModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
    {
         return Qt::ItemIsDropEnabled;
    }
    else if (index.column() == 1)
    {
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return Qt::NoItemFlags;
}

}

