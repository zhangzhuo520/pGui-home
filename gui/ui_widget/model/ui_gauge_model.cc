#include "ui_gauge_model.h"

namespace ui {

GaugeModel::GaugeModel(QObject *parent)
{
      setParent(parent);
}

GaugeModel::~GaugeModel()
{
}


void GaugeModel::setHeaderList(const QString &headerlist)
{
    QStringList temp_list = headerlist.split(QRegExp("\\s+|\t"),  QString::SkipEmptyParts);
    if (temp_list.count() < 1)
    {
        logger_console.debug("Header data Error !");
        return;
    }
    m_header_list = temp_list;
    reset();
}

void GaugeModel::set_data_list(const QStringList & DataList)
{
    QStringList temp_list = DataList.at(0).split(QRegExp("\\s+|\t"),  QString::SkipEmptyParts); // Using regular expressions instand of  "empty" and "Tab"
    if (temp_list.count() < 0)
    {
        logger_console.debug("File data Error !");
        return;
    }

    m_data_list = DataList;
    reset();
}

void GaugeModel::clear_data_list()
{
    m_data_list.clear();
    reset();
}

QModelIndex GaugeModel::parent(const QModelIndex &child)const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QModelIndex GaugeModel::index(int row, int column, const QModelIndex &parent) const
{
       return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex();
}

int GaugeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data_list.count();
}

int GaugeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_header_list.count();
}

QVariant GaugeModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
    {
        return QVariant();
    }
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;

    if((Qt::DisplayRole == role))
    {
        QStringList list = m_data_list.at(item.row()).split(QRegExp("\\s+|\t"),  QString::SkipEmptyParts);
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

QVariant GaugeModel::headerData(int section, Qt::Orientation orientation, int role)const
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
