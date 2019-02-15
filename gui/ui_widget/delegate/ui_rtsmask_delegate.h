#ifndef UI_RTSMASK_DELEGATE_H
#define UI_RTSMASK_DELEGATE_H
#include <QItemDelegate>
#include <QComboBox>
#include <QStringList>
namespace ui {
class RtsMaskDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    RtsMaskDelegate(QObject *parent = 0, const QStringList & list = QStringList());
    ~RtsMaskDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QStringList m_commbox_list;
};
}

#endif // UI_RTSMASK_DELEGATE_H
