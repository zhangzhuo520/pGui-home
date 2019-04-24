#include "ui_rtsreview_table.h"
namespace ui {

RtsReviewTable::RtsReviewTable(QWidget *parent, const QStringList & HeadList):
    QTableView(parent)
{
    m_rts_review_model = new UniversalModel(this);
    m_rts_review_model->setHeaderList(HeadList);
    setModel(m_rts_review_model);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    horizontalHeader()->setHighlightSections(false);
    verticalHeader()->hide();
}

RtsReviewTable::~RtsReviewTable()
{
    delete m_rts_review_model;
    m_rts_review_model = 0;
    m_tabel_data_list.clear();
    m_line_data_list.clear();
}

const QList <LineData*>&RtsReviewTable::cutline_list()
{
    return m_line_data_list;
}

void RtsReviewTable::set_cutline_list(const QList <LineData*>& line_list)
{
    m_tabel_data_list.clear();
    m_line_data_list.clear();
    for (int i = 0; i < line_list.count(); i ++)
    {
        QPointF first_point = line_list[i]->m_first_point;
        QPointF last_point = line_list[i]->m_last_point;
        double first_point_x = first_point.x();
        double first_point_y = first_point.y();
        QString x  = "(" + QString::number(first_point_x) + "," + QString::number(first_point_y) + ")";

        double last_point_x = last_point.x();
        double last_point_y = last_point.y();
        QString y  = "(" + QString::number(last_point_x) + "," + QString::number(last_point_y) + ")";

        double center_x = (first_point_x + last_point_x) / 2;
        double center_y = (first_point_y + last_point_y) / 2;
        QString center  = "(" + QString::number(center_x) + "," + QString::number(center_y) + ")";

        QString string = QString::number(m_line_data_list.count()) + "##" + x + "##" + y + "##" + center;
        m_tabel_data_list.append(string);
        m_line_data_list.append(line_list.at(i));
    }
    m_rts_review_model->set_data_list(m_tabel_data_list);
}

void RtsReviewTable::delete_all_data()
{
    m_tabel_data_list.clear();
    m_line_data_list.clear();
    m_rts_review_model->set_data_list(m_tabel_data_list);
}

void RtsReviewTable::delete_data(const int & index)
{
    if (index > -1 && index < m_tabel_data_list.count())
    {
        m_tabel_data_list.removeAt(index);
        m_line_data_list.removeAt(index);
        m_rts_review_model->set_data_list(m_tabel_data_list);
    }
}

int RtsReviewTable::count() const
{
    return m_line_data_list.count();
}

RtsReviewHistoryTable::RtsReviewHistoryTable(QWidget *parent, const QStringList & HeadList):
    QTableView(parent)
{
    m_rts_review_model = new UniversalModel(this);
    m_rts_review_model->setHeaderList(HeadList);
    setModel(m_rts_review_model);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    horizontalHeader()->setHighlightSections(false);
    verticalHeader()->hide();
}

RtsReviewHistoryTable::~RtsReviewHistoryTable()
{
    delete m_rts_review_model;
    m_rts_review_model = 0;
    m_tabel_data_list.clear();
}

void RtsReviewHistoryTable::append_data(const QString & data)
{
    m_tabel_data_list.append(data);
    m_rts_review_model->set_data_list(m_tabel_data_list);
}

void RtsReviewHistoryTable::delete_all_data()
{
    m_tabel_data_list.clear();
    m_rts_review_model->set_data_list(m_tabel_data_list);
}

void RtsReviewHistoryTable::delete_data(const int & index)
{
    if (index > -1 && index < m_tabel_data_list.count())
    {
        m_tabel_data_list.removeAt(index);
        m_rts_review_model->set_data_list(m_tabel_data_list);
    }
}

int RtsReviewHistoryTable::count() const
{
    return m_tabel_data_list.count();
}
}
