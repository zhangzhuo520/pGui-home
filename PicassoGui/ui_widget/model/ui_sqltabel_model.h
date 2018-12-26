#ifndef UI_SQLTABEL_MODEL_H
#define UI_SQLTABEL_MODEL_H
#include <QSqlQueryModel>

namespace ui {
class SqlQueryModel : public QSqlQueryModel
{
public:
    explicit SqlQueryModel(QObject *parent = 0);

    void setquery(const QSqlQuery &query);
protected:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};
}
#endif // UI_SQLTABEL_MODEL_H
