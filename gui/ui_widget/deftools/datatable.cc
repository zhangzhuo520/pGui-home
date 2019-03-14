#include "datatable.h"
namespace ui {

DataTable::DataTable(QWidget *parent, const QStringList & HeadList):
    m_active_index(-1)
{
    setParent(parent);
    m_data_model = new UniversalModel(this);
    m_data_model->setHeaderList(HeadList);
    setModel(m_data_model);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setClickable(false);
    verticalHeader()->hide();
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(slot_context_menu(QPoint)));
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(slot_update_index(QModelIndex)));
}

DataTable::~DataTable()
{
    delete m_data_model;
    m_data_model = 0;
    m_tabel_data_list.clear();
}

void DataTable::append_data(const QString & data)
{
    m_tabel_data_list.append(data);
    m_data_model->set_data_list(m_tabel_data_list);
}

void DataTable::delete_all_data()
{
    m_tabel_data_list.clear();
    m_data_model->set_data_list(m_tabel_data_list);
}

void DataTable::delete_data(const int & index)
{
    if (index > -1 && index < m_tabel_data_list.count())
    {
        m_tabel_data_list.removeAt(index);
        m_data_model->set_data_list(m_tabel_data_list);
    }
}

int DataTable::count() const
{
    return m_tabel_data_list.count();
}

void DataTable::slot_context_menu(QPoint pos)
{
    QMenu clearMenu;
    clearMenu.addAction("delete", this, SLOT(slot_delete_row()));
    clearMenu.addAction("delete All", this, SLOT(slot_delete_all()));

    QPoint tempPos = static_cast <QWidget *> (parent())->mapToGlobal(pos);
    tempPos = QPoint(tempPos.x() + 2, tempPos.y() + 40);
    clearMenu.exec(tempPos);
}

void DataTable::slot_delete_row()
{
    if (m_active_index != -1)
    {
        m_data_model->delete_line(m_active_index);
    }
}

void DataTable::slot_delete_all()
{
    delete_all_data();
}

void DataTable::slot_update_index(QModelIndex index)
{
    if (index.isValid())
    {
        m_active_index = index.row();
    }
}
}
