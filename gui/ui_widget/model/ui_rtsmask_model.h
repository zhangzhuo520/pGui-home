#ifndef UI_RTSMASK_MODEL_H
#define UI_RTSMASK_MODEL_H
#include <QAbstractItemModel>
#include <QStringList>

class RtsMaskModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    RtsMaskModel(QObject * parent = 0);
    ~RtsMaskModel();

    void append_row(const QString &);
    void delete_row(const int &);
    void delete_all();
    virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole)const;
private:
    void set_data_list(const QStringList &);
    QStringList m_data_list;
    QStringList m_header_list;
    QStringList m_alias_list;
};

#endif // UI_RTSMASK_MODEL_H
