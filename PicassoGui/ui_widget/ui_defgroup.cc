#include "ui_defgroup.h"

namespace ui {
DefGroup::DefGroup(QWidget *parent , QString Path, QModelIndex *defectGroupId, int job) :
    QWidget(parent),
    DbPath(Path),
    index(defectGroupId),
    totalCount(0),
    jobIndex(job)
{
    setWindowTitle("Job" + QString::number(jobIndex) + "_defects");
    setObjectName("Job" + QString::number(jobIndex) + "_defects");
    initSql();
    initOtherButton();
    initDefGroupTable();
    showDefects(index);
    addLayout();
}

void DefGroup::initDefGroupTable()
{
    DefGroupTable = new QTableView(this);
    DefGroupModel = new SqlQueryModel(DefGroupTable);
    //When selected, select the entire row and allow only one row to be selected
    DefGroupTable->setShowGrid(false);
    DefGroupTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    DefGroupTable->setSelectionMode(QAbstractItemView::SingleSelection);
//    DefGroupTable->horizontalHeader()->setClickable(false);
    DefGroupTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    DefGroupTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    DefGroupTable->verticalHeader()->setDefaultSectionSize(20);
    DefGroupTable->verticalHeader()->setMinimumSectionSize(20);
    DefGroupTable->verticalHeader()->hide();

    DefGroupTable->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
//    DefGroupTable->horizontalHeader()->setSortIndicator(2, Qt::AscendingOrder);
//    DefGroupTable->horizontalHeader()->setSortIndicator(3, Qt::AscendingOrder);
    DefGroupTable->horizontalHeader()->setSortIndicatorShown(true);
    DefGroupTable->horizontalHeader()->setClickable(true);
    connect(DefGroupTable->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT (slot_sort_by_column(int, Qt::SortOrder)));

    connect(DefGroupTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_showDefects(QModelIndex)));
}

void DefGroup::initOtherButton()
{
//    Buttonbar = new QWidget(this);
//    Buttonbar->setMinimumWidth(10);
//    sortLable = new QLabel("sort :", Buttonbar);
//    sortCombox = new Commbox(Buttonbar);
//    sortCombox->addItems(QStringList() <<"worst_size" << "defect_number" << "detDefGroup_id");
//    descentButton = new QRadioButton("Descent", Buttonbar);
//    ascentButton = new QRadioButton("Ascent",Buttonbar);
//    if(!descentButton->isChecked())
//    {
//        descentButton->setChecked(true);
//        ascentButton->setChecked(false);
//    }
//    else
//    {
//        descentButton->setChecked(false);
//        ascentButton->setChecked(true);
//    }
    pervButton = new QPushButton("Perv Page" ,this);
    nextButton = new QPushButton("Next Page", this);
    m_pagecount_label = new QLabel(this);

//    connect(descentButton, SIGNAL(clicked()), this, SLOT(slot_descentButtonCheck()));
//    connect(ascentButton, SIGNAL(clicked()), this, SLOT(slot_ascentButtonCheck()));
//    connect(sortCombox, SIGNAL(currentIndexChanged(QString)), SLOT(slot_changSortQrder(QString)));
    connect(pervButton, SIGNAL(clicked()), this, SLOT(slot_pervPage()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(slot_nextPage()));
}

void DefGroup::addLayout()
{
    m_vlayout = new QVBoxLayout(this);
    m_vlayout->addWidget(DefGroupTable);

//    QHBoxLayout* ButtonBarLayout = new QHBoxLayout(this);
//    Hlayout1 = new QHBoxLayout();
//    ButtonBarLayout->addWidget(sortLable);
//    ButtonBarLayout->addWidget(sortCombox);
//    ButtonBarLayout->addWidget(descentButton);
//    ButtonBarLayout->addWidget(ascentButton);
//    ButtonBarLayout->setContentsMargins(0, 0, 0, 0);
//    Buttonbar->setLayout(ButtonBarLayout);
//    Vlayout->addWidget(Buttonbar);
    m_hlayout1 = new QHBoxLayout();
    m_hlayout1->addWidget(pervButton);
    m_hlayout1->addWidget(nextButton);
    m_hlayout1->addWidget(m_pagecount_label);
    m_hlayout1->setContentsMargins(0, 0, 0, 0);
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

    DefectGroupQuery->setData(DefectGroupData);

    DefGroupModel->setquery(DefectGroupQuery->outputSQL());
    DefGroupTable->setModel(DefGroupModel);
    sqlManager->closeDB();
}

void DefGroup::openDB()
{
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

//void DefGroup::slot_DefGroupUpdata(QModelIndex *index)
//{
//    showDefects(index);
//}

//void DefGroup::slot_changSortQrder(QString order)
//{
//    DefectGroupData.orderBy = order;
//    DefectGroupQuery->setData(DefectGroupData);
//    DefGroupModel->setQuery(DefectGroupQuery->outputSQL());
//    updataTable();
//}

//void DefGroup::slot_descentButtonCheck()
//{
//    if(descentButton->isDown())
//    {
//        descentButton->setDown(false);
//        ascentButton->setDown(true);
//    }
//    else
//    {
//        descentButton->setDown(true);
//        ascentButton->setDown(false);
//    }
//    setData();
//}

//void DefGroup::slot_ascentButtonCheck()
//{
//    if(ascentButton->isDown())
//    {
//        descentButton->setDown(true);
//        ascentButton->setDown(false);
//    }
//    else
//    {
//        descentButton->setDown(false);
//        ascentButton->setDown(true);
//    }
//    setData();
//}

void DefGroup::slot_pervPage()
{
    nextButton->setEnabled(true);
    if (DefectGroupData.limitIndex.toInt() - DefectGroupData.pageCount.toInt() <= 5)
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
    upDataPage();
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

    DefectGroupData.limitIndex = QString::number(currIndex);
    upDataPage();
}

void DefGroup::slot_sort_by_column(int index, Qt::SortOrder sort_order)
{
    switch (index) {
    case 1:
    {
        DefectGroupData.orderBy = "detDefGroup_id";
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
        DefectGroupData.orderBy = "defect_number";
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

        break;
    }
    default:
        break;
    }
    setData();
}

void DefGroup::update_page()
{
    QString str = QString::number(CurrentPage)  + "/" + DefectGroupData.pageCount;
    m_pagecount_label->setText(str);
}

void DefGroup::setData()
{
    Q_UNUSED(groupId);
    DefectGroupData.table_id = QString::number(tableId);
    DefectGroupData.pageCount = "10";
    CurrentPage = 0;
    DefectGroupData.limitIndex = QString::number(CurrentPage);
    updataTable();
}

void DefGroup::setTotal()
{
    CountGroupData.tableName = "detector_defect_group";
    CountGroupData.table_id = QString::number(tableId);
    CountGroupQuery->setData(CountGroupData);
    QSqlQuery query = CountGroupQuery->outputSQL();
    while(query.next())
    {
        totalCount = query.value(0).toLongLong();
    }
}

void DefGroup::upDataPage()
{
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
}

void DefGroup::resizeColumns()
{
    for(int i = 0; i < headerList.count(); i ++)
    {
           DefGroupTable->resizeColumnToContents(i);
    }
}

void DefGroup::slot_showDefects(QModelIndex index)
{
    emit signal_showDefects(index, jobIndex);
}
}
