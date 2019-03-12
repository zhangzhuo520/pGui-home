#include "ui_defgroup.h"

namespace ui {

DefGroup::DefGroup(DockWidget *parent , QString Path, QModelIndex *defectGroupId, int job) :
    DbPath(Path),
    index(defectGroupId),
    totalCount(0),
    jobIndex(job),
    m_each_page_count(10)
{
    setParent(parent);
    setWindowTitle("Job" + QString::number(jobIndex) + "_category");
    setObjectName("Job" + QString::number(jobIndex) + "_category");

    initSql();
    initOtherButton();
    initDefGroupTable();
    showDefects(index);
    addLayout();
    initContextMenu();
}

void DefGroup::initDefGroupTable()
{
    DefGroupTable = new QTableView(this);
    DefGroupModel = new SqlQueryModel(DefGroupTable);
    //When selected, select the entire row and allow only one row to be selected
    DefGroupTable->setShowGrid(false);
    DefGroupTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    DefGroupTable->setSelectionMode(QAbstractItemView::SingleSelection);

    DefGroupTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    DefGroupTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    DefGroupTable->horizontalHeader()->setHighlightSections(false);
    DefGroupTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    DefGroupTable->verticalHeader()->setDefaultSectionSize(20);
    DefGroupTable->verticalHeader()->setMinimumSectionSize(20);
    DefGroupTable->verticalHeader()->hide();

    DefGroupTable->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

    DefGroupTable->horizontalHeader()->setSortIndicatorShown(true);
    DefGroupTable->horizontalHeader()->setClickable(true);
    connect(DefGroupTable->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT (slot_sort_by_column(int, Qt::SortOrder)));

    connect(DefGroupTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_showDefects(QModelIndex)));
}

void DefGroup::initOtherButton()
{

    pervButton = new QPushButton(this);
    pervButton->setIcon(QIcon(":/dfjy/images/last_page.png"));
    nextButton = new QPushButton(this);
    nextButton->setIcon(QIcon(":/dfjy/images/next_page.png"));
    m_pagecount_label = new QLabel(this);
    m_page_jump_edit = new PageJumpEdit(this);

    connect(pervButton, SIGNAL(clicked()), this, SLOT(slot_pervPage()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(slot_nextPage()));
    connect(m_page_jump_edit, SIGNAL(signal_jump(QString)), this, SLOT(slot_jump_page(QString)));
}

void DefGroup::addLayout()
{
    m_vlayout = new QVBoxLayout(this);
    m_vlayout->addWidget(DefGroupTable);

    m_hlayout1 = new QHBoxLayout();
    m_hlayout1->addWidget(pervButton);
    m_hlayout1->addWidget(m_pagecount_label);
    m_hlayout1->addWidget(m_page_jump_edit);
    m_hlayout1->addWidget(nextButton);

    m_hlayout1->setContentsMargins(0, 0, 0, 0);
    m_hlayout1->setStretch(0, 1);
    m_hlayout1->setStretch(1, 1);
    m_hlayout1->setStretch(2, 1);
    m_hlayout1->setStretch(3, 1);
    m_vlayout->addLayout(m_hlayout1);
    m_vlayout->setContentsMargins(2, 2, 2, 2);
    setLayout(m_vlayout);
}

void DefGroup::initSql()
{
    DefectGroupQuery = new defectGroupSQL();
    CountGroupQuery = new CountGroupSQL();
    sqlManager = new SQLManager();
}

void DefGroup::updataTable()
{
    logger_widget("Reading database");
    openDB();
    setTotal();
    if(totalCount <= DefectGroupData.pageCount.toInt())
    {
        pervButton->setEnabled(false);
        nextButton->setEnabled(false);
    }
    else
    {
        nextButton->setEnabled(true);
        pervButton->setEnabled(true);
    }

    if (CurrentPage == 1)
    {
        pervButton->setEnabled(false);
    }

    DefectGroupQuery->setData(DefectGroupData);

    DefGroupModel->setquery(DefectGroupQuery->outputSQL());
    DefGroupTable->setModel(DefGroupModel);
    sqlManager->closeDB();
    update_page_number();
    logger_widget("Read database completion");
}

void DefGroup::openDB()
{
    qDebug() << DbPath;
    if (!DbPath.isEmpty())
    {
        sqlManager->setDatabaseName(DbPath);
    }
    else
    {
        qDebug() << "DbPath is Empty";
    }
    if(!sqlManager->openDB())
    {
        qDebug() << "DB open Failed";
    }
}

void DefGroup::showDefects(QModelIndex *index)
{
    QModelIndex tableIdIndex = index->sibling(index->row(), 9);
    QModelIndex groupIdIndex = index->sibling(index->row(), 2);

    tableId = tableIdIndex.data().toInt();
    groupId = groupIdIndex.data().toInt();
    DefectGroupData.orderBy = "worst_size";
    DefectGroupData.order = "asc";
    setData();
}

void DefGroup::updata_all_data(QModelIndex *index)
{
        showDefects(index);
}

void DefGroup::slot_pervPage()
{
    nextButton->setEnabled(true);
    if (DefectGroupData.limitIndex.toInt() - DefectGroupData.pageCount.toInt() < DefectGroupData.pageCount.toInt())
    {
        DefectGroupData.limitIndex = "0";
        pervButton->setEnabled(false);
    }
    else
    {
        pervButton->setEnabled(true);
        DefectGroupData.limitIndex = QString::number(DefectGroupData.limitIndex.toInt() - DefectGroupData.pageCount.toInt());
    }
    if (DefectGroupData.limitIndex.toInt() <= 0)
        DefectGroupData.limitIndex = "0";
    CurrentPage = CurrentPage - 1;
    update_page();
}

void DefGroup::slot_nextPage()
{
    pervButton->setEnabled(true);
    qlonglong prevIndex = DefectGroupData.limitIndex.toInt();
    qlonglong pageCount = DefectGroupData.pageCount.toInt();
    qlonglong currIndex = prevIndex + DefectGroupData.pageCount.toInt();

    if((currIndex + pageCount) >= totalCount)
    {
        nextButton->setDisabled(true);
    }
    else
    {
        nextButton->setDisabled(false);
    }
    CurrentPage = CurrentPage + 1;
    DefectGroupData.limitIndex = QString::number(currIndex);
    update_page();
}

void DefGroup::slot_jump_page(QString page_number)
{
    jump_page(page_number.toInt());
}

void DefGroup::slot_sort_by_column(int index, Qt::SortOrder sort_order)
{
    switch (index) {
    case 1:
    {
        DefectGroupData.orderBy = "category_id";
        if (sort_order == Qt::AscendingOrder)
        {
            DefectGroupData.order = "asc";
        }
        else
        {
            DefectGroupData.order = "desc";
        }
        break;
    }
    case 2:
    {
        DefectGroupData.orderBy = "defect_count";
        if (sort_order == Qt::AscendingOrder)
        {
            DefectGroupData.order = "asc";
        }
        else
        {
            DefectGroupData.order = "desc";
        }
        break;
    }
    case 3:
    {
        DefectGroupData.orderBy = "worst_size";
        if (sort_order == Qt::AscendingOrder)
        {
            DefectGroupData.order = "asc";
        }
        else
        {
            DefectGroupData.order = "desc";
        }
        break;
    }
    default:
        break;
    }
    setData();
}

void DefGroup::slot_custom_contextmenu(QPoint point)
{
    QMenu *menu = new QMenu(this);
    QAction *each_page_count_action = new QAction("Set number of display per page", menu);
    menu->addAction(each_page_count_action);
    connect(each_page_count_action, SIGNAL(triggered()), this, SLOT(slot_set_page_count()));
    point.setY(point.y() + 20);  //acction position is too up
    menu->exec(DefGroupTable->mapToGlobal(point));
}

void DefGroup::slot_set_page_count()
{
    SettingDialog setDialog(this, "Number:", "", QString::number(m_each_page_count, 'f', 0));
    setDialog.setWindowTitle("Set number of display per page");
    if (setDialog.exec())
    {
        if (setDialog.get_button_flag())
        {
            if (setDialog.get_input_data().toInt() <= 0||setDialog.get_input_data().toInt() >= 200)
            {
                showWarning(this, "Waring", "Set range overrun!");
                return;
            }
            m_each_page_count = setDialog.get_input_data().toInt();
            setData();
        }
    }
    else
    {
        return;
    }
}

void DefGroup::update_page_number()
{
    if (totalCount % m_each_page_count == 0)
    {
        QString str = QString::number(CurrentPage)  + "/" + QString::number(totalCount / m_each_page_count);
        m_pagecount_label->setText(str);
    }
    else
    {
        QString str = QString::number(CurrentPage)  + "/" + QString::number(totalCount / m_each_page_count + 1);
        m_pagecount_label->setText(str);
    }
}

void DefGroup::jump_page(int page_number)
{
    if (page_number < 1 ||
            ((page_number > (totalCount / m_each_page_count + 1)) &&
             (page_number * 10 != totalCount)))
    {
        return;
    }

    if (page_number == 1)
    {
        nextButton->setDisabled(false);
        pervButton->setDisabled(true);
    }
    else if(page_number > 1 &&
            ((page_number <  (totalCount / m_each_page_count + 1)) &&
            (page_number * 10 != totalCount)))
    {
        nextButton->setDisabled(false);
        pervButton->setDisabled(false);
    }
    else if((page_number ==  (totalCount / m_each_page_count + 1)) ||
            (page_number * 10 == totalCount))
    {
        nextButton->setDisabled(true);
        pervButton->setDisabled(false);
    }

    qlonglong currIndex = (page_number - 1) * 10;
    DefectGroupData.limitIndex = QString::number(currIndex);
    CurrentPage = page_number;
    update_page();
}

void DefGroup::setData()
{
    Q_UNUSED(groupId);
    DefectGroupData.table_id = QString::number(tableId);
    DefectGroupData.pageCount = QString::number(m_each_page_count);
    CurrentPage = 1;
    DefectGroupData.limitIndex = "0";
    updataTable();
}

void DefGroup::setTotal()
{
    CountGroupData.tableName = "category";
    CountGroupData.table_id = QString::number(tableId);
    CountGroupQuery->setData(CountGroupData);
    QSqlQuery query = CountGroupQuery->outputSQL();
    while(query.next())
    {
        totalCount = query.value(0).toLongLong();
    }
}

void DefGroup::update_page()
{
    logger_widget("Reading database");
    openDB();
    setTotal();
    DefectGroupQuery->setData(DefectGroupData);

    DefGroupModel->setQuery(DefectGroupQuery->outputSQL());
    DefGroupTable->setModel(DefGroupModel);
    DefGroupTable->setColumnWidth(0, 150);
    DefGroupTable->setColumnWidth(1, 150);
    DefGroupTable->setColumnWidth(2, 150);
    DefGroupTable->setColumnWidth(3, 150);
    sqlManager->closeDB();
    update_page_number();
    logger_widget("Read database completion");
}

void DefGroup::resizeColumns()
{
    for(int i = 0; i < headerList.count(); i ++)
    {
        DefGroupTable->resizeColumnToContents(i);
    }
}

void DefGroup::initContextMenu()
{
    DefGroupTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(DefGroupTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slot_custom_contextmenu(QPoint)));
}

void DefGroup::slot_showDefects(QModelIndex index)
{
    emit signal_showDefects(index, jobIndex);
}
}
