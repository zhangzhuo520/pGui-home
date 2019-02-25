#include "ui_measuretable_model.h"
namespace ui {

MeasureTableModel::MeasureTableModel(QObject *parent)
{
        setParent(parent);
}

MeasureTableModel::~MeasureTableModel()
{
}

void MeasureTableModel::setHeaderList(const QStringList &headerlist)
{
    m_header_list = headerlist;
}

void MeasureTableModel::delete_line(const int & row)
{
    m_linedata_list.removeAt(row);
    endRemoveColumns();
}

void MeasureTableModel::set_line_list(const QList<LineData> & line_list)
{
    m_linedata_list = line_list;
    reset();
}

QModelIndex MeasureTableModel::parent(const QModelIndex &child)const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QModelIndex MeasureTableModel::index(int row, int column, const QModelIndex &parent) const
{
       return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex();
}

int MeasureTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_linedata_list.count();
}

int MeasureTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_header_list.count();
}

QVariant MeasureTableModel::data(const QModelIndex &item, int role) const
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
            double x =  m_linedata_list.at(item.row()).m_first_point.x();
            double y =  m_linedata_list.at(item.row()).m_first_point.y();

            QString s = '(' + QString::number(x, 'f' ,4) + ", " + QString::number(y,'f', 4) + ')';

            return s;
        }
        case 1:
        {
            double x =  m_linedata_list.at(item.row()).m_last_point.x();
            double y =  m_linedata_list.at(item.row()).m_last_point.y();

            QString s = '(' + QString::number(x, 'f' ,4) + ", " + QString::number(y,'f', 4) + ')';

            return s;
        }
        case 2:
            return m_linedata_list.at(item.row()).m_distance;
        default:
            break;
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
    else if (role == Qt::ToolTipRole)
    {
        switch (item.column()) {
        case 0:
        {
            double x =  m_linedata_list.at(item.row()).m_first_point.x();
            double y =  m_linedata_list.at(item.row()).m_first_point.y();

            QString s = '(' + QString::number(x, 'f' ,4) + ", " + QString::number(y,'f', 4) + ')';

            return s;
        }
        case 1:
        {
            double x =  m_linedata_list.at(item.row()).m_last_point.x();
            double y =  m_linedata_list.at(item.row()).m_last_point.y();

            QString s = '(' + QString::number(x, 'f' ,4) + ", " + QString::number(y,'f', 4) + ')';
            return s;
        }
        case 2:
            return m_linedata_list.at(item.row()).m_distance;
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

QVariant MeasureTableModel::headerData(int section, Qt::Orientation orientation, int role)const
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
        case 2:
            return m_header_list.at(2);
        case 3:
            return m_header_list.at(3);
        default:
            break;
        }
    }
    return QVariant();
}
}

