#include "ui_rtsmask_delegate.h"
#include <QDebug>
namespace ui {
RtsMaskDelegate::RtsMaskDelegate(QObject *parent, const QStringList & list)
{
    Q_UNUSED(parent);
    m_commbox_list = list;
}

RtsMaskDelegate::~RtsMaskDelegate()
{
}

QWidget *RtsMaskDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        QComboBox* box = new QComboBox(parent);
        box->addItems(m_commbox_list);
        return box;
}

void RtsMaskDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox* box = static_cast<QComboBox*>(editor);
    for (int i = 0; i < m_commbox_list.count(); i ++)
    {
        if (value == m_commbox_list.at(i))
        {
            box->setCurrentIndex(i);
        }
    }
}

void RtsMaskDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* box = static_cast<QComboBox*>(editor);
    model->setData(index, box->currentText(), Qt::DisplayRole);
}

void RtsMaskDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
}
