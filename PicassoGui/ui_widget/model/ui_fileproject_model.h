#ifndef FILEPROJECTMODEL_H
#define FILEPROJECTMODEL_H

#include <QAbstractTableModel>

#include "render_layout_view.h"
#include "../deftools/global.h"

namespace ui
{
class FileProjectModel:public QAbstractTableModel
{
Q_OBJECT
public:

    FileProjectModel(QObject* parent = 0);

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool insertRow(int position, render::LayoutView& lv, const QModelIndex& index = QModelIndex());
    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());
    void removeFile(int row);
    bool find_file(QString);

    std::vector<render::LayoutView>::iterator get_layout_view_iter(int index)
    {
        return m_layout_views.begin() + index;
    }

private:
    std::vector<render::LayoutView> m_layout_views;

};

}
#endif // FILEPROJECTMODEL_H
