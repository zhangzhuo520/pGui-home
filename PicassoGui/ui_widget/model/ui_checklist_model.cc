#include "ui_checklist_model.h"
namespace ui {
TreeModel::TreeModel(QObject *parent)
{
    setParent(parent);
}

QVariant TreeModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
    {
        return QVariant();
    }
    QVariant value = QStandardItemModel::data(item, role);
    switch (role) {
    case Qt::DisplayRole:
        return value;
    case Qt::TextAlignmentRole:
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    case Qt::BackgroundColorRole :
        if (item.row() % 2)
        {
            return QColor(Qt::lightGray);
        }
        else
        {
            return QColor(Qt::white);
        }
    case Qt::SizeHintRole:
        return QSize(32, 16);
    default:
        return value;
    }
    return QVariant();
}
}
