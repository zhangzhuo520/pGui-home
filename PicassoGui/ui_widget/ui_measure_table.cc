#include "ui_measure_table.h"

namespace ui{

MeasureTable::MeasureTable(QWidget *parent)
{
    init();
}

void MeasureTable::init()
{
    m_table_view = new QTableView(this);
    m_table_model = new MeasureTableModel(m_table_view);
    m_button_delegate = new ButtonDelegate(this);
    m_table_view->setModel(m_table_model);

    QStringList header_list;
    header_list << "startPos" << "endPos" << "result";
    m_table_model->setHeaderList(header_list);
    m_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table_view->horizontalHeader()->setClickable(false);
    m_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table_view->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_table_view->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_table_view->horizontalHeader()->setHighlightSections(false);
    m_table_view->horizontalHeader()->setClickable(false);

    m_table_view->verticalHeader()->setDefaultSectionSize(20);
    m_table_view->verticalHeader()->setMinimumSectionSize(20);
    m_table_view->verticalHeader()->hide();
    setWidget(m_table_view);
}

void MeasureTable::set_line_list(QList <LineData> line_list)
{
    m_table_model->set_line_list(line_list);
}
}
