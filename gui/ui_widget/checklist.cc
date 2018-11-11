#include "checklist.h"

CheckList::CheckList(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *Vayout = new QVBoxLayout(this);
    initToolbar();
    initTreeView();
    Vayout->addWidget(CheckListToolbar);
    Vayout->addWidget(CheckListTree);
    Vayout->setSpacing(0);
    Vayout->setContentsMargins(0, 0, 0, 0);
    sqlmanager = new SQLManager();
}

void CheckList::initToolbar()
{
    CheckListToolbar = new QToolBar(this);
    CheckListToolbar->setAutoFillBackground(true);
    QPalette pale = CheckListToolbar->palette();
    pale.setColor(QPalette::Base, UiStyle::ToolBarColor);
    CheckListToolbar->setPalette(pale);
    CheckListCommbox = new Commbox(CheckListToolbar);
    CheckListCommbox->addItem("Defect Centic(Cond.->Mask.->Det.)");
    CheckListCommbox->addItem("Defect Centic(Mask.->Cond.->Det.");
    CheckListCommbox->addItem("Defect Centic");

    CheckListCommbox->setMaximumWidth(100);
    btn_rename = new PushButton(this);
    btn_rename->setIcon(QIcon(":/dfjy/images/rename.png"));
    btn_delete = new PushButton(this);
    btn_delete->setIcon(QIcon(":/dfjy/images/delete.png"));
    btn_load = new PushButton(this);
    btn_load->setIcon(QIcon(":/dfjy/images/load.png"));
    btn_save = new PushButton(this);
    btn_save->setIcon(QIcon(":/dfjy/images/save.png"));
    btn_prev = new PushButton(this);
    btn_prev->setIcon(QIcon(":/dfjy/images/left.png"));
    btn_next = new PushButton(this);
    btn_next->setIcon(QIcon(":/dfjy/images/right.png"));

    CheckListToolbar->addWidget(CheckListCommbox);
    CheckListToolbar->addWidget(btn_rename);
    CheckListToolbar->addWidget(btn_delete);
    CheckListToolbar->addWidget(btn_load);
    CheckListToolbar->addWidget(btn_save);
    CheckListToolbar->addWidget(btn_prev);
    CheckListToolbar->addWidget(btn_next);
    CheckListToolbar->layout()->setSpacing(0);
    CheckListToolbar->layout()->setContentsMargins(0, 0, 0, 0);
}

void CheckList::initTreeView()
{
    CheckListTree = new QTreeView(this);
    CheckListModel = new QStandardItemModel(CheckListTree);

    CheckListTree->setModel(CheckListModel);
    headerList << "Name" << "Detector_table_id" << "Focus" << "Dose" << "Bias" << "Type"
               << "Count"<< "Groups" << "Loose_groups" << "Defect_range" << "Unit";
    CheckListModel->setHorizontalHeaderLabels(headerList);
    CheckListTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    CheckListTree->setSelectionMode(QAbstractItemView::SingleSelection);
    CheckListTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    CheckListTree->setContextMenuPolicy(Qt::CustomContextMenu);
    CheckListTree->setColumnWidth(0, 200);
    CheckListTree->setColumnWidth(1, 80);
    CheckListTree->setColumnWidth(2, 80);
    CheckListTree->setColumnWidth(3, 80);
    CheckListTree->setColumnWidth(4, 80);
    CheckListTree->setColumnWidth(5, 80);
    CheckListTree->setColumnWidth(6, 80);
    CheckListTree->setColumnWidth(7, 80);
    CheckListTree->setColumnWidth(8, 80);
    CheckListTree->setColumnWidth(9, 80);
    CheckListTree->setColumnWidth(10, 80);
    connect(CheckListTree, SIGNAL(customContextMenuRequested(const QPoint& )), this, SLOT(slot_CheckListContextMenu(const QPoint&)));
    connect(CheckListTree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_showDefGroup(QModelIndex)));
}

void CheckList::updataTreeView()
{
    QStandardItem *rootFileItem = new QStandardItem(jobData);
    CheckListModel->appendRow(rootFileItem);

    QStandardItem* pStandardItem = NULL;
    QStandardItem* pStandardChildItem = NULL;
    QStandardItem* pStandardGrandsonItem = NULL;

    for (int i = 0; i < maskVector.count(); i ++ )
    {
        pStandardItem = new QStandardItem(maskVector.at(i).value("mask_desc"));
        rootFileItem->appendRow(pStandardItem);
        rootFileItem->setChild(pStandardItem->row(), headerList.indexOf("Count") ,new QStandardItem(Count));

        for (int j = 0; j < pw_conditionVector.count(); j ++)
        {
            pStandardChildItem = new QStandardItem(pw_conditionVector.at(j).value("cond_name"));
            pStandardItem->appendRow(pStandardChildItem);
            pStandardItem->setChild(pStandardItem->row(), headerList.indexOf("Count") ,new QStandardItem(Count));
            pStandardItem->setChild(j, headerList.indexOf("Focus") ,new QStandardItem(pw_conditionVector.at(j).value("defocus")));
            pStandardItem->setChild(j, headerList.indexOf("Dose") ,new QStandardItem(pw_conditionVector.at(j).value("dedose")));
            pStandardItem->setChild(j, headerList.indexOf("Bias") ,new QStandardItem(pw_conditionVector.at(j).value("bias")));

            for (int k = 0; k < detectorVector.count(); k ++)
            {
                pStandardGrandsonItem = new QStandardItem(detectorVector.at(k).value("d_name"));
                if (detectorVector.at(k).value("cond1_id") == pw_conditionVector.at(j).value("cond_id"))
                {
                    pStandardChildItem->appendRow(pStandardGrandsonItem);
                    pStandardChildItem->setChild(k, 0 ,pStandardGrandsonItem);
                    pStandardChildItem->setChild(k, headerList.indexOf("Detector_table_id") ,new QStandardItem(detectorVector.at(k).value("detector_table_id")));
                    pStandardChildItem->setChild(k, headerList.indexOf("Type") ,new QStandardItem(detectorVector.at(k).value("d_type")));
                    pStandardChildItem->setChild(k, headerList.indexOf("Groups") ,new QStandardItem(detectorVector.at(k).value("group_num")));
                    pStandardChildItem->setChild(k, headerList.indexOf("Loose_groups") ,new QStandardItem(detectorVector.at(k).value("loose_group_num")));
                    pStandardChildItem->setChild(k, headerList.indexOf("Defect_range") ,new QStandardItem(detectorVector.at(k).value("defrange")));
                    pStandardChildItem->setChild(k, headerList.indexOf("Unit") ,new QStandardItem(detectorVector.at(k).value("unit")));
                }
            }
        }
    }
}

void CheckList::readDB(QString DBname)
{
    // use to save DBdata
    maskMap.clear();
    maskVector.clear();

    pw_conditionMap.clear();
    pw_conditionVector.clear();

    detectorMap.clear();
    detectorVector.clear();
    sqlmanager->setDatabaseName(DBname);

    jobList.append(DBname);

    for(int i = 0; i < jobList.count(); i ++)
    {
        if(DBname == jobList.at(i) && jobList.count() != 1)
        {
            return;
        }
        else
        {
            jobIndex = jobList.count();
        }
    }

    if(sqlmanager->openDB())
    {
         QSqlQuery query;

         jobData = "job" +  QString::number(jobIndex - 1) +":" + DBname;

         //mask data
         QString maskDescColName = "mask_desc";
         QString maskIdColName = "mask_id";

         query.exec("select * from mask");
         QSqlRecord rec = query.record();
         int nameIndex = rec.indexOf(maskDescColName);
         int maskIdIndex = rec.indexOf(maskIdColName);
         while(query.next()){
             maskMap.insert("mask_desc", query.value(nameIndex).toString());
             maskMap.insert("mask_id", query.value(maskIdIndex).toString());
             maskVector.append(maskMap);
         }

         //pw_condition data
         QString pwNameColName = "cond_name";
         QString pwCondIdColName = "cond_id";
         QString pwBiasColName = "bias";
         QString pwDoseColName = "dedose";
         QString pwFocusColName = "defocus";

         query.exec("select * from pw_condition");
         rec = query.record();
         nameIndex = rec.indexOf(pwNameColName);
         int condIdIndex = rec.indexOf(pwCondIdColName);
         int biasIndex = rec.indexOf(pwBiasColName);
         int doseIndex = rec.indexOf(pwDoseColName);
         int focusIndex = rec.indexOf(pwFocusColName);
         while(query.next())
         {
             pw_conditionMap.insert("cond_name", query.value(nameIndex).toString());
             pw_conditionMap.insert("cond_id", query.value(condIdIndex).toString());
             pw_conditionMap.insert("bias", query.value(biasIndex).toString());
             pw_conditionMap.insert("dedose", query.value(doseIndex).toString());
             pw_conditionMap.insert("defocus", query.value(focusIndex).toString());
             pw_conditionVector.append(pw_conditionMap);
         }


         query.exec("select detector_table_id,cond1_id,mask1_id,d_name,d_type, defect_number,group_num,loose_group_num,defrange,unit from detector");
         rec = query.record();


         //detector data
         int detector_table_id = rec.indexOf("detector_table_id");
         int cond1_id = rec.indexOf("cond1_id");
         int mask1_id = rec.indexOf("mask1_id");
         int d_name = rec.indexOf("d_name");
         int d_type = rec.indexOf("d_type");
         int defect_number = rec.indexOf("defect_number");
         int group_num = rec.indexOf("group_num");
         int loose_group_num = rec.indexOf("loose_group_num");
         int defrange = rec.indexOf("defrange");
         int unit = rec.indexOf("unit");
         while(query.next())
         {
             detectorMap.insert("detector_table_id", query.value(detector_table_id).toString());
             detectorMap.insert("cond1_id", query.value(cond1_id).toString());
             detectorMap.insert("mask1_id", query.value(mask1_id).toString());
             detectorMap.insert("d_name", query.value(d_name).toString());
             detectorMap.insert("d_type", query.value(d_type).toString());
             detectorMap.insert("defect_number", query.value(defect_number).toString());
             detectorMap.insert("group_num", query.value(group_num).toString());
             detectorMap.insert("loose_group_num", query.value(loose_group_num).toString());
             detectorMap.insert("defrange", query.value(defrange).toString());
             detectorMap.insert("unit", query.value(unit).toString());
             detectorVector.append(detectorMap);
             qDebug() << query.value(detector_table_id).toString()
                      << query.value(d_name).toString()
                      << query.value(defect_number).toString()
                      << query.value(defrange).toString()
                      << query.value(unit).toString();
         }
        query.exec("select sum(defect_number) from detector where mask1_id and cond1_id");
        while(query.next())
        {
            Count = query.value(0).toString();
        }
    }
    updataTreeView();
    sqlmanager->closeDB();
}

CheckList::~CheckList()
{
}

void CheckList::slot_readDB(QString dbName)
{
    readDB(dbName);
}

void CheckList::slot_CheckListContextMenu(const QPoint& point)
{
    QMenu Checkmenu;
    QAction* FitAction = new QAction(&Checkmenu);
    FitAction ->setText("Fit to Selected Item");

    QAction* PanAction = new QAction(&Checkmenu);
    PanAction->setText("Pan to Selected Item");

    QAction* CopyAction = new QAction(&Checkmenu);
    CopyAction->setText("Copy to Clipboaed");
    CopyAction->setShortcuts(QKeySequence::Copy);

    QAction* HighAction = new QAction(&Checkmenu);
    HighAction->setText("Highlight");

    QAction* ExpandAction = new QAction(&Checkmenu);
    ExpandAction->setText("Expand Items");

    QAction* CollapseAction = new QAction(&Checkmenu);
    CollapseAction->setText("Collapse Items");

    QAction* ColorAction = new QAction(&Checkmenu);
    ColorAction->setText("Color");
    QMenu ColorMenu;
    ColorAction->setMenu(&ColorMenu);

    QAction* TextColorAction = new QAction(&Checkmenu);
    TextColorAction->setText("Text Color");
    QMenu TextColorMenu;
    TextColorAction->setMenu(&TextColorMenu);

    QAction* StyleAction = new QAction(&Checkmenu);
    StyleAction->setText("Style");
    QMenu StyleMenu;
    StyleAction->setMenu(&StyleMenu);

    QAction* TextStyleAction = new QAction(&Checkmenu);
    TextStyleAction->setText("Text Style");
    QMenu TextStyleMenu;
    TextStyleAction->setMenu(&TextStyleMenu);

    QAction* CloneAction = new QAction(&Checkmenu);
    CloneAction->setText("Clone job Results");

    QAction* ViewAction = new QAction(&Checkmenu);
    ViewAction->setText("View job Configuration");

    QAction* OpenAction = new QAction(&Checkmenu);
    OpenAction->setText("Open job PostOPC");

    QAction* CustomAction = new QAction(&Checkmenu);
    CustomAction->setText("Custom Group Query");

    QAction* GroupAction = new QAction(&Checkmenu);
    GroupAction->setText("Set Defect Group Page Size");

    QAction* SetPageAction = new QAction(&Checkmenu);
    SetPageAction->setText("Set Defect Page Size");

    QAction* DeleteAction = new QAction(&Checkmenu);
    DeleteAction->setText("Delete");

    Checkmenu.addAction(FitAction);
    Checkmenu.addAction(PanAction);
    Checkmenu.addAction(CopyAction);
    Checkmenu.addAction(HighAction);
    Checkmenu.addAction(ExpandAction);
    Checkmenu.addAction(CollapseAction);
    Checkmenu.addSeparator();
    Checkmenu.addAction(ColorAction);
    Checkmenu.addAction(TextColorAction);
    Checkmenu.addAction(StyleAction);
    Checkmenu.addAction(TextStyleAction);
    Checkmenu.addSeparator();
    Checkmenu.addAction(CloneAction);
    Checkmenu.addAction(ViewAction);
    Checkmenu.addAction(OpenAction);
    Checkmenu.addAction(CustomAction);
    Checkmenu.addAction(GroupAction);
    Checkmenu.addAction(SetPageAction);
    Checkmenu.addSeparator();
    Checkmenu.addAction(DeleteAction);

    QPoint tempPos = point;
    tempPos.setY(tempPos.y() + 22);
    Checkmenu.exec(CheckListTree->mapToGlobal(tempPos));
}

void CheckList::slot_showDefGroup(QModelIndex index)
{
  emit signal_showDefGroup(index, jobIndex);
    qDebug() << "CheckList QModelIndex : " <<index.data().toString();
}
