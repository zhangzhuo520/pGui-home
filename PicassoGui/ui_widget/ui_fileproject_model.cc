#include "ui_fileproject_model.h"
#include <QDebug>

namespace ui
{

FileProjectModel::FileProjectModel(QObject* parent):QAbstractTableModel(parent)
{

}

int FileProjectModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_layout_views.size();
}

int FileProjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant FileProjectModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }

    if((size_t)index.row() >= m_layout_views.size() || index.row() < 0)
    {
        return QVariant();
    }
    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            render::LayoutView lv = m_layout_views[index.row()];
            const char* s = lv.file_name().c_str();
            return QVariant (s);;
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return QVariant(int(Qt::AlignCenter));
    }
    return QVariant();
}

QVariant FileProjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
    {
        return QVariant();
    }
    if(orientation == Qt::Horizontal)
    {
        switch(section)
        {
            case 0:
                return QVariant("file name");
            default:
                break;
        }
    }
    return QVariant();
}

bool FileProjectModel::insertRow(int position, render::LayoutView& lv, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position);
    std::vector<render::LayoutView>::iterator it = m_layout_views.begin() + position;

    m_layout_views.insert(it, lv);
    endInsertRows();
    return true;
}

bool FileProjectModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);

    beginInsertRows(QModelIndex(), position, position + rows - 1);
    std::vector<render::LayoutView>::iterator it = m_layout_views.begin() + position;

    m_layout_views.insert(it, rows, render::LayoutView());

    endInsertRows();
    return true;
}

bool FileProjectModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    std::vector<render::LayoutView>::iterator it_begin = m_layout_views.begin() + position;
    std::vector<render::LayoutView>::iterator it_end = it_begin + rows;

    for(std::vector<render::LayoutView>::iterator it = it_begin; it != it_end; it++)
    {
        (*it).set_index(-1);
    }

    m_layout_views.erase(it_begin, it_end);
    endRemoveRows();
    return true;
}

void FileProjectModel::removeFile(int row)
{
    removeRow(row);
}

}
