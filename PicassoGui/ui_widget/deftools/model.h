#ifndef MODEL_H
#define MODEL_H
#include <QtSql/QSqlQueryModel>
#include <QColor>
#include <QBrush>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QStandardItemModel>
#include <QStringListModel>
//QStringListModel
namespace ui {
class SqlQueryModel : public QSqlQueryModel
{
public:
    explicit SqlQueryModel(QObject *parent = 0);

    void setquery(const QSqlQuery &query);
protected:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class TreeItem : public QStandardItem
{
public:
    explicit TreeItem(QObject *parent = 0)
    {
        Q_UNUSED(parent);
    }
    TreeItem(QIcon icon)
    {
        setIcon(icon);
        setSizeHint(QSize(32, 24));
    }
    TreeItem(QString text)
    {
        setText(text);
        setSizeHint(QSize(32, 24));
    }
    TreeItem(QColor color)
    {
        setBackground(QBrush(color));
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
#endif // MODEL_H
