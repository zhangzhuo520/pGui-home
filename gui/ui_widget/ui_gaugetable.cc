#include "ui_gaugetable.h"

namespace ui {

GaugeTable::GaugeTable(QWidget *parent):
    QTableView(parent)
{
    init_tableview();
    QHBoxLayout *Hlayout = new QHBoxLayout(this);
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->setSpacing(0);
    Hlayout->addWidget(m_table_view);
    setLayout(Hlayout);
}

void GaugeTable::set_header_list(QString HeaderString)
{
    m_table_model->setHeaderList(HeaderString);
}

void GaugeTable::set_data_list(QStringList DataList)
{
    m_table_model->set_data_list(DataList);
}

void GaugeTable::slot_send_data(QModelIndex index)
{
    emit signal_send_gauge_data(index);
}

void GaugeTable::init_tableview()
{
    m_table_view = new QTableView(this);

    m_table_model = new GaugeModel(m_table_view);

    //When selected, select the entire row and allow only one row to be selected
    m_table_view->setShowGrid(false);
    m_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table_view->horizontalHeader()->setClickable(false);
    m_table_view->horizontalHeader()->setFixedHeight(25);
    m_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table_view->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_table_view->horizontalHeader()->setHighlightSections(false);
    m_table_view->verticalHeader()->hide();
    m_table_view->setModel(m_table_model);
    connect(m_table_view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_send_data(QModelIndex)));
}

}
