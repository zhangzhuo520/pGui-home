#ifndef UI_CHECKLIST_MODEL_H
#define UI_CHECKLIST_MODEL_H
#include <QStandardItemModel>
namespace ui {
class TreeItem : public QStandardItem
{
public:
    explicit TreeItem(QObject *parent = 0)
    {
        Q_UNUSED(parent);
        setEditable(false);

    }
    TreeItem(QIcon icon)
    {
        setIcon(icon);
        setSizeHint(QSize(26, 20));
        setEditable(false);
    }
    TreeItem(QString text)
    {
        setText(text);
        setEditable(false);
    }
    TreeItem(QColor color)
    {
        setBackground(QBrush(color));
        setEditable(false);
    }
};

class TreeModel :public QStandardItemModel
{
public:
       explicit TreeModel(QObject *parent = 0);

protected:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

}
#endif // UI_CHECKLIST_MODEL_H
