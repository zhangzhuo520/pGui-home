#ifndef UI_RTSREVIEW_TABLE_H
#define UI_RTSREVIEW_TABLE_H

#include <QTableView>
#include <QHeaderView>
#include <QStringList>

#include "../model/ui_universal_model.h"
#include "../ui_measurepoint.h"

namespace ui {

class RtsReviewTable : public QTableView
{
    Q_OBJECT
public:
    explicit RtsReviewTable(QWidget *parent, const QStringList & HeadList = QStringList());
    ~RtsReviewTable();

    const QList <LineData*>& cutline_list();
    void set_cutline_list(const QList <LineData*>&);
    void delete_all_data();
    void delete_data(const int&);

    int count() const;

private:
    UniversalModel *m_rts_review_model;
    QStringList m_tabel_data_list;
    QList <LineData*> m_line_data_list;
};

class RtsReviewHistoryTable : public QTableView
{
    Q_OBJECT
public:
    explicit RtsReviewHistoryTable(QWidget *parent, const QStringList & HeadList = QStringList());
    ~RtsReviewHistoryTable();

    void append_data(const QString &);
    void delete_all_data();
    void delete_data(const int&);

    int count() const;

private:
    UniversalModel *m_rts_review_model;
    QStringList m_tabel_data_list;
};
}
#endif // UI_RTSREVIEW_TABLE_H
