#include "ui_rtsmask_delegate.h"
namespace ui {
RtsMaskDelegate::RtsMaskDelegate(QObject *parent)
{
    Q_UNUSED(parent);
}

RtsMaskDelegate::~RtsMaskDelegate()
{
}

QWidget *RtsMaskDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        QComboBox* box = new QComboBox(parent);
        QStringList list;
        list << "1" << "2" << "3";
        box->addItems(list);
        return box;
}

void RtsMaskDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox* box = static_cast<QComboBox*>(editor);
    box->addItem(value);
}

void RtsMaskDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* box = static_cast<QComboBox*>(editor);
    model->setData(index, box->currentText(), Qt::EditRole);
}

void RtsMaskDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
}
