#include "ui_rtsreview_table.h"
namespace ui {

RtsReviewTable::RtsReviewTable(QWidget *parent, const QStringList & HeadList):
    QTableView(parent)
{
    m_rts_review_model = new UniversalModel(this);
    m_rts_review_model->setHeaderList(HeadList);
    setModel(m_rts_review_model);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    horizontalHeader()->setHighlightSections(false);
}

RtsReviewTable::~RtsReviewTable()
{
    delete m_rts_review_model;
    m_rts_review_model = 0;
    m_tabel_data_list.clear();
}

void RtsReviewTable::append_data(const QString & data)
{
    m_tabel_data_list.append(data);
    m_rts_review_model->set_data_list(m_tabel_data_list);
}

void RtsReviewTable::delete_all_data()
{
    m_tabel_data_list.clear();
    m_rts_review_model->set_data_list(m_tabel_data_list);
}

void RtsReviewTable::delete_data(const int & index)
{
    if (index > -1 && index < m_tabel_data_list.count())
    {
        m_tabel_data_list.removeAt(index);
        m_rts_review_model->set_data_list(m_tabel_data_list);
    }
}

int RtsReviewTable::count() const
{
    return m_tabel_data_list.count();
}

}
