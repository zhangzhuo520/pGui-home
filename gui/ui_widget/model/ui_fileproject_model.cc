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
    return m_files_info.count();
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
    if(index.row() >= m_files_info.count() || index.row() < 0)
    {
        return QVariant();
    }
    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            render::LayoutView* lv = m_files_info[index.row()].layout_view;
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

bool FileProjectModel::insertRow(int position, FileInfo file_info, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position);
    files_info_iter it = m_files_info.begin() + position;

    m_files_info.insert(it, file_info);
    endInsertRows();
    return true;
}

//bool FileProjectModel::insertRows(int position, int rows, const QModelIndex &index)
//{
//    Q_UNUSED(index);
//    beginInsertRows(QModelIndex(), position, position + rows - 1);
//    files_info_iter it = m_files_info.begin() + position;

//    FileInfo file_info;
//    m_files_info.insert(it, rows, file_info);

//    endInsertRows();
//    return true;
//}

bool FileProjectModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    files_info_iter it_begin = m_files_info.begin() + position;
    files_info_iter it_end = it_begin + rows;

    for(files_info_iter it = it_begin; it != it_end; it++)
    {
        (*it).layout_view->close();
    }

    m_files_info.erase(it_begin, it_end);
    endRemoveRows();
    return true;
}

void FileProjectModel::removeFile(int row)
{
    removeRows(row, 1);
    reset();
}

void FileProjectModel::delete_File(QString filename)
{
    for (int i = 0; i < m_files_info.count(); i ++)
    {
        if (filename == QString::fromStdString(m_files_info[i].layout_view->file_name()))
        {
            removeFile(i);
        }
    }
}

bool FileProjectModel::find_file(QString filename)
{
    for (int i = 0; i < m_files_info.count(); i ++)
    {
        if (filename == QString::fromStdString(m_files_info[i].layout_view->file_name()))
        {
            return true;
        }
    }
    return false;
}

}
