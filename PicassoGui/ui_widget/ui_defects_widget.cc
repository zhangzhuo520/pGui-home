#include "ui_defects_widget.h"

namespace ui {
DefectsWidget::DefectsWidget(QWidget *parent , QString Path, QModelIndex *defectm_group_id, int job) :
    QWidget(parent),
    m_db_path(Path),
    m_model_index(defectm_group_id),
    m_total_count(0),
    m_jobindex(job)
{
    setWindowTitle("Job" + QString::number(m_jobindex) + "_defects");
    setObjectName("Job" + QString::number(m_jobindex) + "_defects");
    initSql();
    initOtherButton();
    initm_defects_table();
    showDefects(m_model_index);
    addLayout();
}

void DefectsWidget::initm_defects_table()
{
    m_defects_table = new QTableView(this);

    QSizePolicy police = m_defects_table->sizePolicy();
    police.setHorizontalPolicy(QSizePolicy::Preferred);
    m_defects_model = new SqlQueryModel(m_defects_table);

    //When selected, select the entire row and allow only one row to be selected
    m_defects_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_defects_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_defects_table->horizontalHeader()->setClickable(false);
    m_defects_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_defects_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_defects_table->verticalHeader()->setDefaultSectionSize(20);
    m_defects_table->verticalHeader()->setMinimumSectionSize(20);
    m_defects_table->verticalHeader()->hide();
}

void DefectsWidget::initOtherButton()
{
    m_button_bar = new QWidget(this);
    m_button_bar->setMinimumWidth(10);
    m_sort_lable = new QLabel("sort :", m_button_bar);
    m_sort_commbox = new Commbox(m_button_bar);
    m_sort_commbox->addItems(QStringList() <<"id" << "x" << "y" << "size");
    m_descent_button = new QRadioButton("Descent", m_button_bar);
    m_ascen_button = new QRadioButton("Ascent",m_button_bar);
    m_extract_button = new PushButton("Extract", this);
    m_moreoptions_button = new PushButton("more options", this);
    if(!m_descent_button->isChecked())
    {
        m_descent_button->setChecked(true);
        m_ascen_button->setChecked(false);
    }
    else
    {
        m_descent_button->setChecked(false);
        m_ascen_button->setChecked(true);
    }
    m_perv_button = new PushButton("Perv Page" ,this);
    m_next_button = new PushButton("Next Page", this);
    connect(m_descent_button, SIGNAL(clicked()), this, SLOT(slot_m_descent_buttonCheck()));
    connect(m_ascen_button, SIGNAL(clicked()), this, SLOT(slot_m_ascen_buttonCheck()));
    connect(m_sort_commbox, SIGNAL(currentm_model_indexChanged(QString)), SLOT(slot_changSortQrder(QString)));
    connect(m_perv_button, SIGNAL(clicked()), this, SLOT(slot_pervPage()));
    connect(m_next_button, SIGNAL(clicked()), this, SLOT(slot_nextPage()));
}

void DefectsWidget::addLayout()
{
    m_vlayout = new QVBoxLayout(this);
    m_vlayout->addWidget(m_defects_table);

    m_hlayout_s = new QHBoxLayout();

    QHBoxLayout *m_hlayoutm_button_bar = new QHBoxLayout();
    m_hlayoutm_button_bar->addWidget(m_sort_lable);
    m_hlayoutm_button_bar->addWidget(m_sort_commbox);
    m_hlayoutm_button_bar->addWidget(m_descent_button);
    m_hlayoutm_button_bar->addWidget(m_ascen_button);
    m_hlayoutm_button_bar->addWidget(m_extract_button);
    m_hlayoutm_button_bar->setSpacing(10);
    m_button_bar->setLayout(m_hlayoutm_button_bar);
    m_hlayout_s->addWidget(m_button_bar);
    m_vlayout->addLayout(m_hlayout_s);

    m_vlayout->addWidget(m_moreoptions_button);
    m_hlayout_s = new QHBoxLayout();
    m_hlayout_s->addWidget(m_perv_button);
    m_hlayout_s->addWidget(m_next_button);
    m_vlayout->addLayout(m_hlayout_s);
    m_vlayout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_vlayout);
}

void DefectsWidget::initSql()
{
    m_defects_query = new defectSQL();
    m_count_defects_query = new CountDefectSQL();
    m_sqlmanager = new SQLManager();
}

void DefectsWidget::updataTable()
{
    openDB();
    setTotal();
    if(m_total_count <= m_defect_data.pageCount.toInt())
    {
        m_perv_button->setEnabled(false);
        m_next_button->setEnabled(false);
    }
    else
    {
        m_next_button->setEnabled(true);
        m_perv_button->setEnabled(true);
    }

    m_defects_query->setData(m_defect_data);

    m_defects_model->setQuery(m_defects_query->outputSQL());
    m_defects_table->setModel(m_defects_model);
    m_defects_table->setColumnWidth(0, 150);
    m_defects_table->setColumnWidth(1, 150);
    m_defects_table->setColumnWidth(2, 150);
    m_defects_table->setColumnWidth(3, 150);
    m_defects_table->setColumnWidth(4, 150);
    m_defects_table->setColumnWidth(5, 150);
    m_defects_table->setColumnWidth(6, 150);
    m_sqlmanager->closeDB();
}

void DefectsWidget::openDB()
{
    if (!m_db_path.isEmpty())
    {
        m_sqlmanager->setDatabaseName(m_db_path);
    }
    else
    {
        qDebug() << "m_db_path is Empty";
    }
    if(!m_sqlmanager->openDB())
    {
        qDebug() << "DB open Failed";
    }
}

void DefectsWidget::changeSortKey()
{
}

void DefectsWidget::resizeColumns()
{
}

void DefectsWidget::sortTable()
{
}

void DefectsWidget::setData()
{
    m_defect_data.table_id = QString::number(m_table_id);
    m_defect_data.detdefgroup_id = QString::number(m_group_id);
    m_defect_data.pageCount = "10";
    m_defect_data.orderBy = m_sort_commbox->currentText();
    m_current_page = 0;
    if(m_descent_button->isChecked())
    {
        m_defect_data.order = "desc";
    }
    else
    {
        m_defect_data.order = "asc";
    }
    m_defect_data.limitIndex = QString::number(m_current_page);
    updataTable();
}

void DefectsWidget::setTotal()
{
    m_countdefect_data.tableName = "defect";
    m_countdefect_data.table_id = QString::number(m_table_id);
    m_countdefect_data.defGroup_id = QString::number(m_group_id);
    m_count_defects_query->setData(m_countdefect_data);
    QSqlQuery query = m_count_defects_query->outputSQL();
    while(query.next())
    {
        m_total_count = query.value(0).toLongLong();
    }
}

void DefectsWidget::upDataPage()
{
    openDB();
    setTotal();
    m_defects_query->setData(m_defect_data);

    m_defects_model->setQuery(m_defects_query->outputSQL());
    m_defects_table->setModel(m_defects_model);
    m_defects_table->setColumnWidth(0, 150);
    m_defects_table->setColumnWidth(1, 150);
    m_defects_table->setColumnWidth(2, 150);
    m_defects_table->setColumnWidth(3, 150);
    m_defects_table->setColumnWidth(4, 150);
    m_defects_table->setColumnWidth(5, 150);
    m_defects_table->setColumnWidth(6, 150);
    m_sqlmanager->closeDB();
}

void DefectsWidget::showDefects(QModelIndex *index)
{
     m_table_id = index->sibling(index->row(), 0).data().toInt();
     m_group_id = index->sibling(index->row(), 1).data().toInt();
     setData();
}

QTableView *DefectsWidget::getTableView()
{
    return m_defects_table;
}

void DefectsWidget::slot_defectsUpdata(QModelIndex *index)
{
    showDefects(index);
}

void DefectsWidget::slot_changSortQrder(QString order)
{
    m_defect_data.orderBy = order;
    m_defects_query->setData(m_defect_data);
    m_defects_model->setQuery(m_defects_query->outputSQL());
    updataTable();
}

void DefectsWidget::slot_m_descent_buttonCheck()
{
    if(m_descent_button->isDown())
    {
        m_descent_button->setDown(false);
        m_ascen_button->setDown(true);
    }
    else
    {
        m_descent_button->setDown(true);
        m_ascen_button->setDown(false);
    }
    setData();
}

void DefectsWidget::slot_m_ascen_buttonCheck()
{
    if(m_ascen_button->isDown())
    {
        m_descent_button->setDown(true);
        m_ascen_button->setDown(false);
    }
    else
    {
        m_descent_button->setDown(false);
        m_ascen_button->setDown(true);
    }
    setData();
}

void DefectsWidget::slot_pervPage()
{
    m_next_button->setEnabled(true);
    if (m_defect_data.limitIndex.toInt() - m_defect_data.pageCount.toInt() <= 5)
    {
        m_defect_data.limitIndex = "0";
        m_perv_button->setEnabled(false);
    }
    else
    {
        m_perv_button->setEnabled(true);
        m_defect_data.limitIndex = QString::number(m_defect_data.limitIndex.toInt() - m_defect_data.pageCount.toInt());
    }
    if (m_defect_data.limitIndex.toInt() <= 0)
        m_defect_data.limitIndex = "0";
    upDataPage();
}

void DefectsWidget::slot_nextPage()
{
    m_perv_button->setEnabled(true);
    qlonglong prevIndex = m_defect_data.limitIndex.toInt();
    qlonglong pageCount = m_defect_data.pageCount.toInt();
    qlonglong currIndex = prevIndex + m_defect_data.pageCount.toInt();

    if((currIndex + pageCount) >= m_total_count)
    {
        m_next_button->setDisabled(true);
    }
    else
    {
        m_next_button->setDisabled(false);
    }

    m_defect_data.limitIndex = QString::number(currIndex);
    upDataPage();
}

void DefectsWidget::slot_showDefects(QModelIndex *index)
{
    emit signal_showDefects(index);
}
}

