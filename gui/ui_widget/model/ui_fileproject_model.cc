#include "ui_fileproject_model.h"
#include <QDebug>

namespace ui
{

FileProjectModel::FileProjectModel(QObject* parent)
{
    setParent(parent);
}

QModelIndex FileProjectModel::parent(const QModelIndex &child)const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QModelIndex FileProjectModel::index(int row, int column, const QModelIndex &parent)const
{
    return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex();
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
            render::LayoutView* lv = m_layout_views[index.row()];
            QString file_name = QString::fromStdString(lv->file_name());
            QStringList list = file_name.split("/", QString::SkipEmptyParts);
            QString abbr_file_name = list.back();
            return QVariant (abbr_file_name);
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return QVariant(int(Qt::AlignLeft | Qt::AlignVCenter));
    }
    else if (role == Qt::SizeHintRole)
    {
        return QSize(100, 15);
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

bool FileProjectModel::insertRow(int position, render::LayoutView* lv, const QModelIndex& index)
{
    qDebug() << QString::fromStdString((*lv).file_name());
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position);
    layout_view_iter it = m_layout_views.begin() + position;

    m_layout_views.insert(it, lv);
    endInsertRows();
    return true;
}

bool FileProjectModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);
    layout_view_iter it = m_layout_views.begin() + position;

    m_layout_views.insert(it, rows, new render::LayoutView());

    endInsertRows();
    return true;
}

bool FileProjectModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    std::vector<render::LayoutView*>::iterator it_begin = m_layout_views.begin() + position;
    std::vector<render::LayoutView*>::iterator it_end = it_begin + rows;

    for(layout_view_iter it = it_begin; it != it_end; it++)
    {
        (*it)->close();
        delete *it;
        *it = 0;
    }

    m_layout_views.erase(it_begin, it_end);
    endRemoveRows();
    return true;
}

void FileProjectModel::removeFile(int row)
{
    removeRows(row, 1);
}

void FileProjectModel::delete_File(QString filename)
{
    for (uint i = 0; i < m_layout_views.size(); i ++)
    {
        if (filename == QString::fromStdString(m_layout_views.at(i)->file_name()))
        {
            removeRows(i, 1);
        }
    }
}

bool FileProjectModel::find_file(QString filename)
{
    for (uint i = 0; i < m_layout_views.size(); i ++)
    {
        if (filename == QString::fromStdString(m_layout_views.at(i)->file_name()))
        {
            return true;
        }
    }
    return false;
}

}
