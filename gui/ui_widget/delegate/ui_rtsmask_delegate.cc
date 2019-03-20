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

void RtsMaskDelegate::set_commbox_list(const QStringList & list)
{
    m_commbox_list = list;
}

QSize RtsMaskDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    return QSize(size.width(), 10);
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

void RtsMaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, QColor(128, 171, 220));

    QString text = index.data(Qt::DisplayRole).toString();
    if(!text.isEmpty())
    {
        painter->save();
        painter->setPen(QColor(10, 10, 10));
        if (text.length() > 1 && (text.length() * 7) > option.rect.right())
        {
            int lenght = option.rect.right() / 19; // (9 * 2   9: 7pixcel/char  2 : start and end)
            text =  text.left(lenght) + "..." + text.right(lenght);
        }
        QPoint Point = QPoint(option.rect.left() + 15, option.rect.top() + 17);
        painter->drawText(Point, text);
        painter->restore();
    }
}
}
