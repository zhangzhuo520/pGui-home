#ifndef FILEPROJECTMODEL_H
#define FILEPROJECTMODEL_H

#include <QAbstractItemModel>
#include <QDebug>

#include "render_layout_view.h"
#include "render_frame.h"
#include "../deftools/global.h"
#include "../deftools/datastruct.h"

namespace ui
{
class FileProjectModel:public QAbstractItemModel
{
    Q_OBJECT


public:
    typedef typename QVector<FileInfo>::iterator files_info_iter;

    FileProjectModel(QObject* parent = 0);
    virtual QModelIndex parent(const QModelIndex &child)const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex())const;

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool insertRow(int position, FileInfo, const QModelIndex& index = QModelIndex());
//    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());
    void removeFile(int row);
    void delete_File(QString);
    bool find_file(QString);

    files_info_iter get_files_info_iter(int index)
    {
        return m_files_info.begin() + index;
    }

private:
    QVector<FileInfo> m_files_info;

};

}
#endif // FILEPROJECTMODEL_H
