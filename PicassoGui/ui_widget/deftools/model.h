#ifndef MODEL_H
#define MODEL_H
#include <QtSql/QSqlQueryModel>
#include <QColor>
#include <QBrush>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QStandardItemModel>


namespace UI {
class SqlQueryModel : public QSqlQueryModel
{
public:
    explicit SqlQueryModel(QObject *parent = 0);

    void setquery(const QSqlQuery &query);
protected:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class LayerTreeModel :public QStandardItemModel
{
public:
       explicit LayerTreeModel(QObject *parent = 0);

protected:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};
}
#endif // MODEL_H
