#ifndef UI_RTSREVIEW_TABLE_H
#define UI_RTSREVIEW_TABLE_H

#include <QTableView>
#include <QHeaderView>
#include <QStringList>

#include "../model/ui_universal_model.h"

namespace ui {

class RtsReviewTable : public QTableView
{
    Q_OBJECT
public:
    explicit RtsReviewTable(QWidget *parent, const QStringList & HeadList = QStringList());
    ~RtsReviewTable();

    void append_data(const QString &);

    void delete_data();

private:
    UniversalModel *m_rts_review_model;
    QStringList m_tabel_data_list;
};
}
#endif // UI_RTSREVIEW_TABLE_H
