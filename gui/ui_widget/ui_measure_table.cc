#include "ui_measure_table.h"

namespace ui{

MeasureTable::MeasureTable(QWidget *parent):
    m_active_index(-1)
{
    setParent(parent);
    init();
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(slot_context_menu(QPoint)));
}

void MeasureTable::init()
{
    QHBoxLayout *Hlayout = new QHBoxLayout(this);
    m_table_view = new QTableView(this);
    m_table_model = new MeasureTableModel(m_table_view);
    m_table_view->setModel(m_table_model);

    QStringList header_list;
    header_list << "startPos" << "endPos" << "result";
    m_table_model->setHeaderList(header_list);
    m_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table_view->horizontalHeader()->setClickable(false);
    m_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table_view->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_table_view->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_table_view->horizontalHeader()->setHighlightSections(false);
    m_table_view->horizontalHeader()->setClickable(false);

    m_table_view->verticalHeader()->setDefaultSectionSize(20);
    m_table_view->verticalHeader()->setMinimumSectionSize(20);
    m_table_view->verticalHeader()->hide();
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->addWidget(m_table_view);

    connect(m_table_view, SIGNAL(clicked(QModelIndex)), this, SLOT(slot_update_index(QModelIndex)));
    connect(m_table_view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_checked_line(QModelIndex)));
}

void MeasureTable::only_clear_table_data()
{
    m_linedata_list.clear();
    m_table_model->set_line_list(m_linedata_list);
}

//void MeasureTable::set_line_list(QList <LineData> line_list)
//{
//    m_linedata_list = line_list;
//    m_table_model->set_line_list(line_list);
//}

void MeasureTable::slot_context_menu(QPoint pos)
{
    QMenu clearMenu;
    clearMenu.addAction("delete", this, SLOT(slot_delete_row()));
    clearMenu.addAction("delete All", this, SLOT(slot_delete_all()));
    clearMenu.exec(m_table_view->mapToGlobal(pos));
}

void MeasureTable::slot_delete_row()
{
    if (m_active_index != -1)
    {
        m_linedata_list.removeAt(m_active_index);
        m_table_model->set_line_list(m_linedata_list);
        emit signal_set_line_list(m_linedata_list);
    }
}

void MeasureTable::slot_delete_all()
{
    m_linedata_list.clear();
    m_table_model->set_line_list(m_linedata_list);
    emit signal_set_line_list(m_linedata_list);
}

void MeasureTable::slot_update_index(QModelIndex index)
{
    m_active_index = index.row();
}

void MeasureTable::slot_checked_line(QModelIndex index)
{
    int listMaxIndex = m_linedata_list.count() - 1;
    if (listMaxIndex < index.row())
    {
        qDebug() << "Measure line list count number less!";
        return;
    }

//    QList <LineData>* temp_linedata_list = m_linedata_list;
//    for (int i = 0; i < m_linedata_list.count(); i ++)
//    {
//        if (m_linedata_list[i]->m_line_color == Qt::red)
//        {
//            temp_linedata_list[i]->set_line_color(Qt::black);
//        }

//        if (i == index.row())
//        {
//            temp_linedata_list[i]->set_line_color(Qt::red);  //set line color is red represent is line has focus
//        }
//    }
    QPointF start_point(0, 0);
    QPointF end_point(0, 0);
    for(int i = 0; i < m_linedata_list.count(); i++)
    {
        if(m_linedata_list[i]->m_line_color == Qt::red)
        {
            m_linedata_list[i]->set_line_color(Qt::black);
        }
        if( i == index.row())
        {
            m_linedata_list[i]->set_line_color(Qt::red);
            start_point = m_linedata_list[i]->m_first_point;
            end_point = m_linedata_list[i]->m_last_point;
        }
    }

    QPointF center_point((start_point.x() + end_point.x()) / 2, (start_point.y() + end_point.y()) / 2);



    emit signal_set_line_list(m_linedata_list);
    emit signal_move_center(center_point);
}

void MeasureTable::slot_set_line_list(const QList<LineData*> &linedata_list)
{
    m_linedata_list = linedata_list;
    m_table_model->set_line_list(linedata_list);
}

void MeasureTable::clear_select_color()
{
    for (int i = 0; i < m_linedata_list.count(); i ++)
    {
        m_linedata_list[i]->set_line_color(Qt::black);
    }
}

}
