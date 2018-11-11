#include "defectswidget.h"

DefectsWidget::DefectsWidget(QWidget *parent , QString Path, QModelIndex *defectGroupId, int job) :
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
    initdefectsTable();
    showDefects(index);
    addLayout();
}

void DefectsWidget::initdefectsTable()
{
    defectsTable = new QTableView(this);

    QSizePolicy police = defectsTable->sizePolicy();
    police.setHorizontalPolicy(QSizePolicy::Preferred);
    defectsModel = new SqlQueryModel(defectsTable);

    //When selected, select the entire row and allow only one row to be selected
    defectsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    defectsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    defectsTable->horizontalHeader()->setClickable(false);
    defectsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    defectsTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    defectsTable->verticalHeader()->setDefaultSectionSize(20);
    defectsTable->verticalHeader()->setMinimumSectionSize(20);
    defectsTable->verticalHeader()->hide();
}

void DefectsWidget::initOtherButton()
{
    Buttonbar = new QWidget(this);
    Buttonbar->setMinimumWidth(10);
    sortLable = new QLabel("sort :", Buttonbar);
    sortCombox = new Commbox(Buttonbar);
    sortCombox->addItems(QStringList() <<"id" << "x" << "y" << "size");
    descentButton = new QRadioButton("Descent", Buttonbar);
    ascentButton = new QRadioButton("Ascent",Buttonbar);
    ExtractButton = new PushButton("Extract", this);
    MoreOptionsButton = new PushButton("more options", this);
    if(!descentButton->isChecked())
    {
        descentButton->setChecked(true);
        ascentButton->setChecked(false);
    }
    else
    {
        descentButton->setChecked(false);
        ascentButton->setChecked(true);
    }
    pervButton = new PushButton("Perv Page" ,this);
    nextButton = new PushButton("Next Page", this);
    pervButton->setStyleSheet(UiStyle::ButtonStyle);
    nextButton->setStyleSheet(UiStyle::ButtonStyle);
    ExtractButton->setStyleSheet(UiStyle::ButtonStyle);
    MoreOptionsButton->setStyleSheet(UiStyle::ButtonStyle);
    connect(descentButton, SIGNAL(clicked()), this, SLOT(slot_descentButtonCheck()));
    connect(ascentButton, SIGNAL(clicked()), this, SLOT(slot_ascentButtonCheck()));
    connect(sortCombox, SIGNAL(currentIndexChanged(QString)), SLOT(slot_changSortQrder(QString)));
    connect(pervButton, SIGNAL(clicked()), this, SLOT(slot_pervPage()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(slot_nextPage()));
}

void DefectsWidget::addLayout()
{
    Vlayout = new QVBoxLayout(this);
    Vlayout->addWidget(defectsTable);

    Hlayout1 = new QHBoxLayout();

    QHBoxLayout *HlayoutButtonBar = new QHBoxLayout();
    HlayoutButtonBar->addWidget(sortLable);
    HlayoutButtonBar->addWidget(sortCombox);
    HlayoutButtonBar->addWidget(descentButton);
    HlayoutButtonBar->addWidget(ascentButton);
    HlayoutButtonBar->addWidget(ExtractButton);
    HlayoutButtonBar->setSpacing(10);
    Buttonbar->setLayout(HlayoutButtonBar);
    Hlayout1->addWidget(Buttonbar);
    Vlayout->addLayout(Hlayout1);

    Vlayout->addWidget(MoreOptionsButton);
    Hlayout1 = new QHBoxLayout();
    Hlayout1->addWidget(pervButton);
    Hlayout1->addWidget(nextButton);
    Vlayout->addLayout(Hlayout1);
    Vlayout->setContentsMargins(0, 0, 0, 0);
    setLayout(Vlayout);
}

void DefectsWidget::initSql()
{
    DefectsQuery = new defectSQL();
    CountDefectsQuery = new CountDefectSQL();
    sqlManager = new SQLManager();
}

void DefectsWidget::updataTable()
{
    openDB();
    setTotal();
    if(totalCount <= DefectsData.pageCount.toInt())
    {
        pervButton->setEnabled(false);
        nextButton->setEnabled(false);
    }
    else
    {
        nextButton->setEnabled(true);
        pervButton->setEnabled(true);
    }

    DefectsQuery->setData(DefectsData);

    defectsModel->setQuery(DefectsQuery->outputSQL());
    defectsTable->setModel(defectsModel);
    defectsTable->setColumnWidth(0, 150);
    defectsTable->setColumnWidth(1, 150);
    defectsTable->setColumnWidth(2, 150);
    defectsTable->setColumnWidth(3, 150);
    defectsTable->setColumnWidth(4, 150);
    defectsTable->setColumnWidth(5, 150);
    defectsTable->setColumnWidth(6, 150);
    sqlManager->closeDB();
}

void DefectsWidget::openDB()
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
        MyDebug
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
    DefectsData.table_id = QString::number(tableId);
    DefectsData.detdefgroup_id = QString::number(groupId);
    DefectsData.pageCount = "10";
    DefectsData.orderBy = sortCombox->currentText();
    CurrentPage = 0;
    if(descentButton->isChecked())
    {
        DefectsData.order = "desc";
    }
    else
    {
        DefectsData.order = "asc";
    }
    DefectsData.limitIndex = QString::number(CurrentPage);
    updataTable();
}

void DefectsWidget::setTotal()
{
    CountDefectData.tableName = "defect";
    CountDefectData.table_id = QString::number(tableId);
    CountDefectData.defGroup_id = QString::number(groupId);
    CountDefectsQuery->setData(CountDefectData);
    QSqlQuery query = CountDefectsQuery->outputSQL();
    while(query.next())
    {
        totalCount = query.value(0).toLongLong();
    }
}

void DefectsWidget::upDataPage()
{
    openDB();
    setTotal();
    DefectsQuery->setData(DefectsData);

    defectsModel->setQuery(DefectsQuery->outputSQL());
    defectsTable->setModel(defectsModel);
    defectsTable->setColumnWidth(0, 150);
    defectsTable->setColumnWidth(1, 150);
    defectsTable->setColumnWidth(2, 150);
    defectsTable->setColumnWidth(3, 150);
    defectsTable->setColumnWidth(4, 150);
    defectsTable->setColumnWidth(5, 150);
    defectsTable->setColumnWidth(6, 150);
    sqlManager->closeDB();
}

void DefectsWidget::showDefects(QModelIndex *index)
{
     tableId = index->sibling(index->row(), 0).data().toInt();
     groupId = index->sibling(index->row(), 1).data().toInt();
     setData();
}

QTableView *DefectsWidget::getTableView()
{
    return defectsTable;
}

void DefectsWidget::slot_defectsUpdata(QModelIndex *index)
{
    showDefects(index);
}

void DefectsWidget::slot_changSortQrder(QString order)
{
    DefectsData.orderBy = order;
    DefectsQuery->setData(DefectsData);
    defectsModel->setQuery(DefectsQuery->outputSQL());
    updataTable();
}

void DefectsWidget::slot_descentButtonCheck()
{
    if(descentButton->isDown())
    {
        descentButton->setDown(false);
        ascentButton->setDown(true);
    }
    else
    {
        descentButton->setDown(true);
        ascentButton->setDown(false);
    }
    setData();
}

void DefectsWidget::slot_ascentButtonCheck()
{
    if(ascentButton->isDown())
    {
        descentButton->setDown(true);
        ascentButton->setDown(false);
    }
    else
    {
        descentButton->setDown(false);
        ascentButton->setDown(true);
    }
    setData();
}

void DefectsWidget::slot_pervPage()
{
    nextButton->setEnabled(true);
    if (DefectsData.limitIndex.toInt() - DefectsData.pageCount.toInt() <= 5)
    {
        DefectsData.limitIndex = "0";
        pervButton->setEnabled(false);
    }
    else
    {
        pervButton->setEnabled(true);
        DefectsData.limitIndex = QString::number(DefectsData.limitIndex.toInt() - DefectsData.pageCount.toInt());
    }
    if (DefectsData.limitIndex.toInt() <= 0)
        DefectsData.limitIndex = "0";
    upDataPage();
}

void DefectsWidget::slot_nextPage()
{
    pervButton->setEnabled(true);
    qlonglong prevIndex = DefectsData.limitIndex.toInt();
    qlonglong pageCount = DefectsData.pageCount.toInt();
    qlonglong currIndex = prevIndex + DefectsData.pageCount.toInt();

    if((currIndex + pageCount) >= totalCount)
    {
        nextButton->setDisabled(true);
    }
    else
    {
        nextButton->setDisabled(false);
    }

    DefectsData.limitIndex = QString::number(currIndex);
    upDataPage();
}

void DefectsWidget::slot_showDefects(QModelIndex *index)
{
    emit signal_showDefects(index);
}
