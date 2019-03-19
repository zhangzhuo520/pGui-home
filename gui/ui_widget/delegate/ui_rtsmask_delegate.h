#ifndef UI_RTSMASK_DELEGATE_H
#define UI_RTSMASK_DELEGATE_H
#include <QItemDelegate>
#include <QComboBox>
#include <QStringList>
#include <QPainter>

#include "../qt_logger/pgui_log_global.h"
namespace ui {
class RtsMaskDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    RtsMaskDelegate(QObject *parent = 0, const QStringList & list = QStringList());
    ~RtsMaskDelegate();
    void set_commbox_list(const QStringList &);

    QSize sizeHint(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
private:
    QStringList m_commbox_list;
};
}

#endif // UI_RTSMASK_DELEGATE_H
