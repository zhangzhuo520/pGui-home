#ifndef UI_RTSREVIEW_MODEL_H
#define UI_RTSREVIEW_MODEL_H
#include <QAbstractItemModel>
#include <QStringList>
#include <QColor>
#include <QBrush>

#include "../qt_logger/pgui_log_global.h"

namespace ui {

class UniversalModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit UniversalModel(QObject * parent = 0);
    ~UniversalModel();

    void setHeaderList(const QStringList &);
    void delete_line(const int &);
    void set_data_list(const QStringList &);
    void clear_data_list();
    virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole)const;
private:
   QStringList m_header_list;
   QStringList m_data_list;
};

}
#endif // UI_RTSREVIEW_MODEL_H
