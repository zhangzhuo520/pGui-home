#include "ui_checklist.h"

namespace ui {
CheckList::CheckList(int width, int height, QWidget *parent):
    QWidget(parent),
    m_width(width),
    m_height(height),
    m_active_tree_index(0)
{
    m_vlayout = new QVBoxLayout(this);
    initToolbar();
    initTreeView();
    initFoundWidget();
    m_vlayout->addWidget(m_checklist_toolbar);
    m_vlayout->addWidget(m_checklist_tree);
    m_vlayout->setSpacing(0);
    m_vlayout->setContentsMargins(0, 0, 0, 0);
    m_sqlmanager = new SQLManager();
}

void CheckList::initToolbar()
{
    m_checklist_toolbar = new Toolbar(this, m_width, m_height);
    m_checklist_toolbar->setAutoFillBackground(true);
    QPalette pale = m_checklist_toolbar->palette();
    pale.setColor(QPalette::Base, UiStyle::ToolBarColor);
    m_checklist_toolbar->setPalette(pale);
    m_checklist_commbox = new Commbox(m_checklist_toolbar);
    m_checklist_commbox->addItem("Defect Centic(Cond.->Mask.->Det.)");
    m_checklist_commbox->addItem("Defect Centic(Mask.->Cond.->Det.");
    m_checklist_commbox->addItem("Defect Centic");


    m_rename_button = new PushButton(this);
    m_rename_button->setIcon(QIcon(":/dfjy/images/rename.png"));
    m_delete_button = new PushButton(this);
    m_delete_button->setIcon(QIcon(":/dfjy/images/delete.png"));
    m_load_button = new PushButton(this);
    m_load_button->setIcon(QIcon(":/dfjy/images/load.png"));
    m_save_button = new MenuButton(this);
    m_save_button->setIcon(QIcon(":/dfjy/images/save.png"));
    m_prev_button = new PushButton(this);
    m_prev_button->setIcon(QIcon(":/dfjy/images/left.png"));
    m_next_button = new PushButton(this);
    m_next_button->setIcon(QIcon(":/dfjy/images/right.png"));
    m_found_button = new PushButton(this);
    m_found_button->setIcon(QIcon(":/dfjy/images/found.png"));
    m_found_button->setCheckable(true);
    m_rtsconfig_button = new PushButton("RTS-Config", this);
    m_runrts_button = new PushButton("Run-RTS", this);
    m_closejob_button = new PushButton("Close-job", this);
    m_appendjob_button = new PushButton("Append-job", this);
    m_setting_button = new PushButton("Settings", this);

//    m_checklist_toolbar->addWidget(m_checklist_commbox);
//    m_checklist_toolbar->addWidget(m_rename_button);
    m_checklist_toolbar->addWidget(m_delete_button);
    m_checklist_toolbar->addWidget(m_load_button);
    m_checklist_toolbar->addWidget(m_save_button);
    m_checklist_toolbar->addSeparator();
    m_checklist_toolbar->addWidget(m_prev_button);
    m_checklist_toolbar->addWidget(m_next_button);
    m_checklist_toolbar->addWidget(m_found_button);
//    m_checklist_toolbar->addSeparator();
//    m_checklist_toolbar->addWidget(m_rtsconfig_button);
//    m_checklist_toolbar->addWidget(m_runrts_button);
//    m_checklist_toolbar->addWidget(m_closejob_button);
//    m_checklist_toolbar->addWidget(m_appendjob_button);
//    m_checklist_toolbar->addWidget(m_setting_button);

//    m_checklist_toolbar->layout()->setSpacing(5);
//    m_checklist_toolbar->layout()->setContentsMargins(1, 1, 1, 1);

    QMenu *saveMenu = new QMenu(this);
    m_save_button->setMenu(saveMenu);
    QAction *saveLvckAction = new QAction("Save CheckList Lvck", saveMenu);
    QAction *saveCsvAction = new QAction("Export to CSV File", saveMenu);
    saveMenu->addAction(saveLvckAction);
    saveMenu->addAction(saveCsvAction);

    connect(m_rename_button, SIGNAL(clicked()), this, SLOT(slot_BtnRename()));
    connect(m_delete_button, SIGNAL(clicked()), this, SLOT(slot_BtnDelete()));
    connect(m_load_button, SIGNAL(clicked()), this, SLOT(slot_BtnLoad()));
    connect(m_save_button, SIGNAL(signal_leftClick()), this, SLOT(slot_BtnSave()), Qt::UniqueConnection);
    connect(saveLvckAction, SIGNAL(triggered()), this, SLOT(slot_BtnSave()), Qt::UniqueConnection);
    connect(saveCsvAction, SIGNAL(triggered()), this, SLOT(slot_ExportCSV()), Qt::UniqueConnection);
    connect(m_found_button, SIGNAL(clicked(bool)), this, SLOT(slot_BtnFound(bool)), Qt::UniqueConnection);
}

void CheckList::initFoundWidget()
{
    m_findwidget = new QWidget(this);
    m_hlayout = new QHBoxLayout(m_findwidget);
    m_hlayout->setSpacing(0);
    m_hlayout->setContentsMargins(0, 0, 0, 0);
    m_findedit = new QLineEdit(m_findwidget);
    m_infound_button = new PushButton(m_findwidget);
    m_infound_button->setIcon(QIcon(":/dfjy/images/found.png"));

    m_hlayout->addWidget(m_findedit);
    m_hlayout->addWidget(m_infound_button);
   // m_hlayout->addSpacerItem(new QSpacerItem(20, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));
    m_findwidget->hide();
}

void CheckList::initTreeView()
{
    m_checklist_tree = new QTreeView(this);
    m_checklist_model = new TreeModel(m_checklist_tree);

    m_checklist_tree->setModel(m_checklist_model);
    m_headerlist << "Name" << "Type" << "Delta-Focus" << "Delta-Dose" << "Bias"
               << "Defect No"<< "Group No" << "Size_range" << "Unit" << "Detector_table_id";
    m_checklist_model->setHorizontalHeaderLabels(m_headerlist);
    m_checklist_tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_checklist_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_checklist_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_checklist_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_checklist_tree->setColumnWidth(0, 200);
    m_checklist_tree->setColumnWidth(1, 80);
    m_checklist_tree->setColumnWidth(2, 80);
    m_checklist_tree->setColumnWidth(3, 80);
    m_checklist_tree->setColumnWidth(4, 80);
    m_checklist_tree->setColumnWidth(5, 80);
    m_checklist_tree->setColumnWidth(6, 80);
    m_checklist_tree->setColumnWidth(7, 80);
    m_checklist_tree->setColumnWidth(8, 80);
    m_checklist_tree->setColumnWidth(9, 80);
    connect(m_checklist_tree, SIGNAL(customContextMenuRequested(const QPoint& )), this, SLOT(slot_CheckListContextMenu(const QPoint&)));
    connect(m_checklist_tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_showDefGroup(QModelIndex)));
    connect(m_checklist_tree, SIGNAL(clicked(QModelIndex)), this, SLOT(slot_update_current_index(QModelIndex)));
}

void CheckList::updataTreeView()
{
    QStandardItem *rootFileItem = new TreeItem(m_jobdata);
    m_checklist_model->appendRow(rootFileItem);

    QStandardItem* pStandardItem = NULL;
    QStandardItem* pStandardChildItem = NULL;
    QStandardItem* pStandardGrandsonItem = NULL;

    for (int i = 0; i < m_maskvector.count(); i ++ )
    {
        if (!m_maskvector.at(i).value("mask_desc").isEmpty())
        {
            pStandardItem = new TreeItem(m_maskvector.at(i).value("mask_desc"));
            rootFileItem->appendRow(pStandardItem);
            rootFileItem->setChild(i, m_headerlist.indexOf("Defect No") ,new TreeItem(m_count));
        }
        else
        {
            pStandardItem = new TreeItem("Mask Id :" + m_maskvector.at(i).value("mask_id"));
            rootFileItem->appendRow(pStandardItem);
            rootFileItem->setChild(i, m_headerlist.indexOf("Defect No") ,new TreeItem(m_count));
        }

        for (int j = 0; j < m_pw_conditionvector.count(); j ++)
        {
            pStandardChildItem = new TreeItem(m_pw_conditionvector.at(j).value("cond_name"));
            pStandardItem->setChild(j,m_headerlist.indexOf("Name"), pStandardChildItem);
            pStandardItem->setChild(j, m_headerlist.indexOf("Defect No") ,new TreeItem(m_pw_conditionvector.at(j).value("Count")));
            pStandardItem->setChild(j, m_headerlist.indexOf("Delta-Focus") ,new TreeItem(m_pw_conditionvector.at(j).value("defocus")));
            pStandardItem->setChild(j, m_headerlist.indexOf("Delta-Dose") ,new TreeItem(m_pw_conditionvector.at(j).value("dedose")));
            pStandardItem->setChild(j, m_headerlist.indexOf("Bias") ,new TreeItem(m_pw_conditionvector.at(j).value("bias")));
            int pStandardChildItem_row = 0;
            for (int k = 0; k < m_detectorvector.count(); k ++)
            {
                if (m_detectorvector.at(k).value("cond1_id") == m_pw_conditionvector.at(j).value("cond_id"))
                {
                    pStandardGrandsonItem = new TreeItem(m_detectorvector.at(k).value("d_name"));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Name"), pStandardGrandsonItem);
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Detector_table_id") ,new TreeItem(m_detectorvector.at(k).value("detector_table_id")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Type") ,new TreeItem(m_detectorvector.at(k).value("d_type")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Defect No") ,new TreeItem(m_detectorvector.at(k).value("defect_number")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Group No") ,new TreeItem(m_detectorvector.at(k).value("group_num")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Size_range") ,new TreeItem(m_detectorvector.at(k).value("defrange")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Unit") ,new TreeItem(m_detectorvector.at(k).value("unit")));
                    pStandardChildItem_row ++;
                }
            }
        }
        break;
    }
}

void CheckList::read_database(QString DBname)
{
    // use to save DBdata
    m_maskmap.clear();
    m_maskvector.clear();

    m_pw_conditionmap.clear();
    m_pw_conditionvector.clear();

    m_detectormap.clear();
    m_detectorvector.clear();
    m_sqlmanager->setDatabaseName(DBname);

    m_jobpath_list.append(DBname);
    m_jobindex = m_jobpath_list.count();

    if(m_sqlmanager->openDB())
    {
         QSqlQuery query;

         m_jobdata = "job" +  QString::number(m_jobindex) +":" + DBname;

         //mask data
         QString maskDescColName = "mask_desc";
         QString maskIdColName = "mask_id";

         query.exec("select * from mask");
         QSqlRecord rec = query.record();
         int nameIndex = rec.indexOf(maskDescColName);
         int maskIdIndex = rec.indexOf(maskIdColName);
         while(query.next()){
             m_maskmap.insert("mask_desc", query.value(nameIndex).toString());
             m_maskmap.insert("mask_id", query.value(maskIdIndex).toString());
             m_maskvector.append(m_maskmap);
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
             m_pw_conditionmap.insert("cond_name", query.value(nameIndex).toString());
             m_pw_conditionmap.insert("cond_id", query.value(condIdIndex).toString());
             m_pw_conditionmap.insert("bias", query.value(biasIndex).toString());
             m_pw_conditionmap.insert("dedose", query.value(doseIndex).toString());
             m_pw_conditionmap.insert("defocus", query.value(focusIndex).toString());
             m_pw_conditionvector.append(m_pw_conditionmap);
         }


         query.exec("select detector_table_id,cond1_id,mask1_id,d_name,d_type, defect_number,group_num,defrange,unit from detector");
         rec = query.record();


         //detector data
         int detector_table_id = rec.indexOf("detector_table_id");
         int cond1_id = rec.indexOf("cond1_id");
         int mask1_id = rec.indexOf("mask1_id");
         int d_name = rec.indexOf("d_name");
         int d_type = rec.indexOf("d_type");
         int defect_number = rec.indexOf("defect_number");
         int group_num = rec.indexOf("group_num");
         int defrange = rec.indexOf("defrange");
         int unit = rec.indexOf("unit");
         while(query.next())
         {
             m_detectormap.insert("detector_table_id", query.value(detector_table_id).toString());
             m_detectormap.insert("cond1_id", query.value(cond1_id).toString());
             m_detectormap.insert("mask1_id", query.value(mask1_id).toString());
             m_detectormap.insert("d_name", query.value(d_name).toString());
             m_detectormap.insert("d_type", query.value(d_type).toString());
             m_detectormap.insert("defect_number", query.value(defect_number).toString());
             m_detectormap.insert("group_num", query.value(group_num).toString());
             m_detectormap.insert("defrange", query.value(defrange).toString());
             m_detectormap.insert("unit", query.value(unit).toString());
             m_detectorvector.append(m_detectormap);
//             qDebug() << query.value(detector_table_id).toString()
//                      << query.value(d_name).toString()
//                      << query.value(defect_number).toString()
//                      << query.value(defrange).toString()
//                      << query.value(unit).toString();
         }
         QVector <QMap < QString , QString> >::iterator pw_iterator = m_pw_conditionvector.begin();
         for (; pw_iterator != m_pw_conditionvector.end(); pw_iterator ++)
         {
             query.exec(QString("select sum(defect_number) from detector where cond1_id = %1;").arg(pw_iterator->value("cond_id")));
             while (query.next())
             {
                 if (query.value(0).toString().isEmpty())
                 {
                     pw_iterator->insert("Count", "0");
                 }
                 else
                 {
                     pw_iterator->insert("Count", query.value(0).toString());
                 }
             }
         }

        query.exec("select sum(defect_number) from detector where mask1_id and cond1_id");
        while(query.next())
        {
            m_count = query.value(0).toString();
        }
    }
    m_sqlmanager->closeDB();
}

CheckList::~CheckList()
{
}

void CheckList::slot_append_job(QString dbName)
{
    read_database(dbName);
    updataTreeView();
}

void CheckList::slot_close_job(QString filename)
{
    QString m_filename = filename.left(filename.size() - 15);
    QString m_jobpath = "";
    for (int i = 0; i < m_jobpath_list.count(); i ++)
    {
        m_jobpath = m_jobpath_list.at(i).left(m_jobpath_list.at(i).count() - 10);
        if (m_jobpath == m_filename)
        {
            remove_job(i);
        }
    }
}

void CheckList::remove_job(int index)
{
    emit signal_close_job(m_active_index_name);
    emit signal_close_database_widget(m_jobindex);
    m_checklist_model->removeRow(index);
    m_jobpath_list.removeAt(index);
}

QModelIndex CheckList::get_current_rootindex(QModelIndex index)
{
    while (!index.parent().data().toString().isEmpty()){
      index = index.parent();
    }
    QStringList list = index.data().toString().split(':');
    if (list.count() > 1)
    {
        m_active_index_name = list.at(1);
        m_jobindex = list.at(0).right(list.at(0).size() - 3).toInt();
    }
    return index;
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
    DeleteAction->setText("Close");

//    Checkmenu.addAction(FitAction);
//    Checkmenu.addAction(PanAction);
//    Checkmenu.addAction(CopyAction);
//    Checkmenu.addAction(HighAction);
//    Checkmenu.addAction(ExpandAction);
//    Checkmenu.addAction(CollapseAction);
//    Checkmenu.addSeparator();
//    Checkmenu.addAction(ColorAction);
//    Checkmenu.addAction(TextColorAction);
//    Checkmenu.addAction(StyleAction);
//    Checkmenu.addAction(TextStyleAction);
//    Checkmenu.addSeparator();
//    Checkmenu.addAction(CloneAction);
//    Checkmenu.addAction(ViewAction);
//    Checkmenu.addAction(OpenAction);
//    Checkmenu.addAction(CustomAction);
//    Checkmenu.addAction(GroupAction);
//    Checkmenu.addAction(SetPageAction);
//    Checkmenu.addSeparator();
    Checkmenu.addAction(DeleteAction);
   connect(DeleteAction, SIGNAL(triggered()), this, SLOT(slot_close_currentjob()));
    QPoint tempPos = point;
    tempPos.setY(tempPos.y() + 22);
    Checkmenu.exec(m_checklist_tree->mapToGlobal(tempPos));


}

void CheckList::slot_update_current_index(QModelIndex index)
{
    m_active_tree_index = get_current_rootindex(index).row();
}

void CheckList::slot_showDefGroup(QModelIndex index)
{
    m_active_tree_index = get_current_rootindex(index).row();
    QString str = get_current_rootindex(index).data().toString().split(":").at(0);
    int int_index = str.right(str.size() - 3).toInt();
    emit signal_showDefGroup(index, int_index);
}

void CheckList::slot_openLVCK(QString)
{
}

void CheckList::slot_BtnRename()
{
    m_rename_dialog = new QDialog(this);
    m_rename_dialog->setWindowTitle("Rename CheckList");
    QPoint point = m_rename_dialog->mapFromGlobal(QPoint(500, 200));
    m_rename_dialog->setGeometry(point.x(), point.y(), 230, 100);
    QLabel *renameLable = new QLabel(m_rename_dialog);
    renameLable->setGeometry(10, 10, 200, 20);
    renameLable->setText("Please enter new name:");
    m_rename_edit = new QLineEdit(m_rename_dialog);
    m_rename_edit->setGeometry(10, 30, 200, 20);
    m_rename_edit->setText(m_checklist_commbox->currentText());
    m_rename_edit->setSelection(0, m_rename_edit->text().length());
    QPushButton *m_rename_buttonEnter = new QPushButton("Ok", m_rename_dialog);
    QPushButton * m_rename_buttonCancel = new QPushButton("Cancel", m_rename_dialog);
    m_rename_buttonEnter->setGeometry(50, 70, 60, 20);
    m_rename_buttonCancel->setGeometry(120, 70, 60, 20);
    m_rename_dialog->show();
    connect(m_rename_buttonCancel, SIGNAL(clicked()), m_rename_dialog, SLOT(close()));
    connect(m_rename_buttonEnter, SIGNAL(clicked()), this, SLOT(slot_RenameOk()));
}

void CheckList::slot_BtnDelete()
{
    m_checklist_commbox->removeItem(m_checklist_commbox->currentIndex());
}

void CheckList::slot_BtnLoad()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Load CheckList File"));
    fileDialog->setDirectory("/home/dfjy/workspace/job");
    fileDialog->setNameFilter(tr("CheckList File(.lvck)"));
    connect(fileDialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_openLVCK(QString)), Qt::UniqueConnection);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setViewMode(QFileDialog::List);
    fileDialog->show();
}

void CheckList::slot_BtnSave()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->getSaveFileName(this,tr("Save File"),"/home",tr("File(*.lvck)"));
}

void CheckList::slot_ExportCSV()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->getSaveFileName(this,tr("Save File"),"/home",tr("File(*.csv)"));
}

void CheckList::slot_BtnFound(bool isPress)
{
    if (isPress)
    {
        m_vlayout->insertWidget(1, m_findwidget);
        m_findwidget->show();
    }
    else
    {
        m_vlayout->removeWidget(m_findwidget);
        m_findwidget->hide();
    }
}

void CheckList::slot_RenameOk()
{
    m_rename_dialog->close();
    m_checklist_commbox->setItemText(m_checklist_commbox->currentIndex(), m_rename_edit->text());
}

void CheckList::slot_close_currentjob()
{
    remove_job(m_active_tree_index);
}
}
