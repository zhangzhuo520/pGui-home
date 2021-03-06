#include "ui_checklist.h"
#include "ui_defgroup.h"
#include "db/sqlmanager.h"
#include "db/sqlindex_thread.h"
#include "model/ui_checklist_model.h"

namespace ui {
CheckList::CheckList(int width, int height, QWidget *parent):
    QWidget(parent),
    m_width(width),
    m_height(height),
    m_active_tree_index(0)
{
    m_vlayout = new QVBoxLayout();
    m_vlayout->setSpacing(0);
    m_vlayout->setContentsMargins(0, 0, 0, 0);

    initTreeView();
    initJobKey();

    m_vlayout->addWidget(m_checklist_tree);
    setLayout(m_vlayout);
    m_sqlmanager = new SQLManager();
    m_sqlindex_thread = new SqlIndexThread(this);
}

void CheckList::initJobKey()
{
    for (int i = 0; i < 10; i ++)
    {
       JobKey job_key;
       job_key.isUse = false;
       job_key.key = i;
       m_jobkey_vector.append(job_key);
    }
}

void CheckList::initTreeView()
{
    m_checklist_tree = new TreeView(this);
    m_checklist_model = new TreeModel(m_checklist_tree);
    m_headerlist << "Name" << "Check_type" << "Delta-Focus" << "Delta-Dose" << "Bias"
                 << "Defect No"<< "Category No" << "Size_range" << "Unit" << "Check_id";

    m_checklist_model->setHorizontalHeaderLabels(m_headerlist);
    m_checklist_tree->setModel(m_checklist_model);

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
    connect(m_checklist_tree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slot_CheckListContextMenu(const QPoint&)));
    connect(m_checklist_tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_showDefGroup(QModelIndex)));
    connect(m_checklist_tree, SIGNAL(clicked(QModelIndex)), this, SLOT(slot_update_current_index(QModelIndex)));
}

void CheckList::new_update_treeview()
{
    if (m_jobdata.isEmpty())
    {
        return;
    }
    QStandardItem *rootFileItem = new TreeItem(m_jobdata);
    m_rootitem_vector.append(rootFileItem);
    m_checklist_model->appendRow(rootFileItem);
    QStandardItem* pStandardItem = NULL;
    QStandardItem* pStandardChildItem = NULL;
    QStandardItem* pStandardGrandsonItem = NULL;

    for (int i = 0; i < m_maskvector.count(); i ++)
    {
        if (!m_maskvector.at(i).value("name").isEmpty())
        {
            pStandardItem = new TreeItem(m_maskvector.at(i).value("name"));
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
            pStandardChildItem = new TreeItem(m_pw_conditionvector.at(j).value("name"));
            pStandardItem->setChild(j,m_headerlist.indexOf("Name"), pStandardChildItem);
            pStandardItem->setChild(j, m_headerlist.indexOf("Defect No") ,new TreeItem(m_pw_conditionvector.at(j).value("Count")));
            pStandardItem->setChild(j, m_headerlist.indexOf("Delta-Focus") ,new TreeItem(m_pw_conditionvector.at(j).value("delta_focus")));
            pStandardItem->setChild(j, m_headerlist.indexOf("Delta-Dose") ,new TreeItem(m_pw_conditionvector.at(j).value("delta_dose")));
            pStandardItem->setChild(j, m_headerlist.indexOf("Bias") ,new TreeItem(m_pw_conditionvector.at(j).value("mask_bias")));
            int pStandardChildItem_row = 0;
            for (int k = 0; k < m_detectorvector.count(); k ++)
            {
                if (m_detectorvector.at(k).value("condition_id") == m_pw_conditionvector.at(j).value("condition_id"))
                {
                    pStandardGrandsonItem = new TreeItem(m_detectorvector.at(k).value("name"));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Name"), pStandardGrandsonItem);
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Check_id") ,new TreeItem(m_detectorvector.at(k).value("check_id")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Check_type") ,new TreeItem(m_detectorvector.at(k).value("check_type")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Defect No") ,new TreeItem(m_detectorvector.at(k).value("defect_count")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Category No") ,new TreeItem(m_detectorvector.at(k).value("category_count")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Size_range") ,new TreeItem(m_detectorvector.at(k).value("check_range")));
                    pStandardChildItem->setChild(pStandardChildItem_row, m_headerlist.indexOf("Unit") ,new TreeItem(m_detectorvector.at(k).value("unit")));
                    pStandardChildItem_row ++;
                }
            }
        }
        break;
    }
}

void CheckList::new_read_database(const QString&DBname)
{
    // use to save DBdata
    m_maskmap.clear();
    m_maskvector.clear();

    m_pw_conditionmap.clear();
    m_pw_conditionvector.clear();

    m_detectormap.clear();
    m_detectorvector.clear();

    m_sqlmanager->setDatabaseName(DBname + "/defectDB.sqlite");

    if(m_sqlmanager->openDB())
    {
         QSqlQuery query;

         m_jobdata = "job" +  QString::number(m_jobindex) + ":" + DBname;

         //mask data
         QString maskDescColName = "name";
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
         QString pwNameColName = "name";
         QString pwCondIdColName = "condition_id";
         QString pwBiasColName = "mask_bias";
         QString pwDoseColName = "delta_dose";
         QString pwFocusColName = "delta_focus";

         query.exec("select * from process_condition");
         rec = query.record();
         nameIndex = rec.indexOf(pwNameColName);
         int condIdIndex = rec.indexOf(pwCondIdColName);
         int biasIndex = rec.indexOf(pwBiasColName);
         int doseIndex = rec.indexOf(pwDoseColName);
         int focusIndex = rec.indexOf(pwFocusColName);
         while(query.next())
         {
             m_pw_conditionmap.insert("name", query.value(nameIndex).toString());
             m_pw_conditionmap.insert("condition_id", query.value(condIdIndex).toString());
             m_pw_conditionmap.insert("mask_bias", query.value(biasIndex).toString());
             m_pw_conditionmap.insert("delta_dose", query.value(doseIndex).toString());
             m_pw_conditionmap.insert("delta_focus", query.value(focusIndex).toString());
             m_pw_conditionvector.append(m_pw_conditionmap);
         }

         query.exec("select check_id, condition_id, mask_id,name,check_type, defect_count,category_count,check_range,unit from checker");
         rec = query.record();


         //detector data
         int detector_table_id = rec.indexOf("check_id");
         int cond1_id = rec.indexOf("condition_id");
         int mask1_id = rec.indexOf("mask_id");
         int d_name = rec.indexOf("name");
         int d_type = rec.indexOf("check_type");
         int defect_number = rec.indexOf("defect_count");
         int group_num = rec.indexOf("category_count");
         int defrange = rec.indexOf("check_range");
         int unit = rec.indexOf("unit");
         while(query.next())
         {
             m_detectormap.insert("check_id", query.value(detector_table_id).toString());
             m_detectormap.insert("condition_id", query.value(cond1_id).toString());
             m_detectormap.insert("mask_id", query.value(mask1_id).toString());
             m_detectormap.insert("name", query.value(d_name).toString());
             m_detectormap.insert("check_type", query.value(d_type).toString());
             m_detectormap.insert("defect_count", query.value(defect_number).toString());
             m_detectormap.insert("category_count", query.value(group_num).toString());
             m_detectormap.insert("check_range", query.value(defrange).toString());
             m_detectormap.insert("unit", query.value(unit).toString());
             m_detectorvector.append(m_detectormap);
         }
         QVector <QMap < QString , QString> >::iterator pw_iterator = m_pw_conditionvector.begin();
         for (; pw_iterator != m_pw_conditionvector.end(); pw_iterator ++)
         {
             query.exec(QString("select sum(defect_count) from checker where condition_id = %1;").arg(pw_iterator->value("condition_id")));
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

        query.exec("select sum(defect_count) from checker where mask_id and condition_id");
        while(query.next())
        {
            m_count = query.value(0).toString();
        }
    }
    m_sqlmanager->closeDB();
}

int CheckList::get_job_key()
{
    QVector <JobKey>::iterator itor = m_jobkey_vector.begin();

    for (; itor != m_jobkey_vector.end(); itor ++)
    {
        if (!(itor->isUse))
        {
            itor->isUse = true;
            return itor->key;
        }
    }

    return -1;
}

void CheckList::close_job_key(int Key)
{
    QVector <JobKey>::iterator itor = m_jobkey_vector.begin();
    for (; itor != m_jobkey_vector.end(); itor ++)
    {
        if ((*itor).key == Key)
        {
            (*itor).isUse = false;
        }
    }
}

CheckList::~CheckList()
{
    m_maskmap.clear();
    m_maskvector.clear();
    m_pw_conditionmap.clear();
    m_pw_conditionvector.clear();
    m_detectormap.clear();
    m_detectorvector.clear();
    m_jobkey_vector.clear();

    for (int i = 0; i < m_rootitem_vector.count(); i ++)
    {
        delete m_rootitem_vector.at(i);
        m_rootitem_vector[i] = 0;
    }
    m_rootitem_vector.clear();

    delete m_sqlindex_thread;
    m_sqlindex_thread = 0;

    delete m_sqlmanager;
    m_sqlmanager = 0;
}

void CheckList::update_current_job(const FrameInfo & frame_info)
{
    setEnabled(true);
    for (auto i = 0; i < m_checklist_model->rowCount(); i ++)
    {
        if (m_rootitem_vector.count() > i)
        {
            m_checklist_tree->setRowHidden(i, m_checklist_model->indexFromItem(m_rootitem_vector.at(i)).parent(), true);
        }
    }

    if (frame_info.file_type == FileType::file)
    {
        setEnabled(false);
        return;
    }
    for(int i = 0; i < frame_info.fileinfo_vector.count(); i ++)
    {
        QString temp_job_path =  QString::fromStdString(frame_info.fileinfo_vector[i].layout_view->file_name());
        temp_job_path = temp_job_path.left(temp_job_path.count() - 15);
        for (auto i = 0; i < m_rootitem_vector.count(); i ++)
        {
            QString job_path = m_rootitem_vector[i]->text();
            QStringList path_list = job_path.split(":");

            try
            {
                if (path_list.count() < 2)
                {
                    throw "path_list error!";
                }
                if (path_list.at(1) == temp_job_path)
                {
                    m_checklist_tree->setRowHidden(i, m_checklist_model->indexFromItem(m_rootitem_vector.at(i)).parent(), false);
                }
            }
            catch(QString)
            {
                return;
            }
        }
    }
}

// when delete file, frame_info is delete before this, so frame_info is empty. so add the flag
void CheckList::delete_job(const FrameInfo & frame_info, const bool & is_file_delete, QVector <fileinfo> all_filetype_vector, const bool & is_close_active_window)
{
    if (all_filetype_vector.count() > m_rootitem_vector.count())  //file add return , don't delete
    {
        return;
    }

    QVector <QStandardItem *> temp_index_vector;
    temp_index_vector.clear();
    for(int i = m_rootitem_vector.count() - 1; i > -1; i --)
    {
        if (!m_checklist_tree->isRowHidden(i, m_checklist_model->indexFromItem(m_rootitem_vector.at(i)).parent()))   //get checklist tree show modelindex
        {
            temp_index_vector.append(m_rootitem_vector.at(i));
        }
    }

    // click paintTab close button, not close file delete
    //reason : when delete file, frame_info is delete before this, so frame_info is empty. so add the flag
    if (!is_file_delete)
    {
        if (is_close_active_window)
        {
            if (frame_info.fileinfo_vector.isEmpty())  //
            {
                for (auto i = 0; i < temp_index_vector.count(); i ++)
                {
                    QString job_path = temp_index_vector[i]->text();
                    QStringList path_list = job_path.split(":");
                    int job_id = path_list.at(0).right(path_list.at(0).count() - 3).toInt();
                    close_job_key(job_id);
                    emit signal_close_database_widget(job_id);
                    int index = m_checklist_model->indexFromItem(temp_index_vector.at(i)).row();
                    m_checklist_model->removeRow(index);
                    m_rootitem_vector.remove(m_rootitem_vector.indexOf(temp_index_vector.at(i)));
                }
            }
            else
            {
                for (auto i = 0; i < temp_index_vector.count(); i ++)
                {
                    bool is_exits = false;
                    QString job_path = temp_index_vector[i]->text();
                    QStringList path_list = job_path.split(":");
                    for(auto j = 0; j < frame_info.fileinfo_vector.count(); j ++)
                    {
                        QString temp_job_path =  QString::fromStdString(frame_info.fileinfo_vector[j].layout_view->file_name());
                        temp_job_path = temp_job_path.left(temp_job_path.count() - 15);
                        try
                        {
                            if (path_list.count() < 2)
                            {
                                throw "path_list error!";
                            }

                            if (path_list.at(1) == temp_job_path)
                            {
                                is_exits = true;
                            }
                        }
                        catch(...)
                        {
                            logger_widget(QString("job_path error: %1").arg(job_path));
                            return;
                        }
                    }

                    if (is_exits)
                    {
                        QString log = "delete :" + path_list.at(1);
                        logger_file (log);
                        int job_id = path_list.at(0).right(path_list.at(0).count() - 3).toInt();
                        int row = m_checklist_model->indexFromItem(temp_index_vector.at(i)).row();
                        m_checklist_model->removeRow(row);
                        close_job_key(job_id);
                        emit signal_close_database_widget(job_id);
                        m_rootitem_vector.remove(m_rootitem_vector.indexOf(temp_index_vector[i]));
                    }
                }
            }
        }
        else
        {
            for (auto i = 0; i < m_rootitem_vector.count(); i ++)
            {
                bool is_exits = false;
                QString job_path = m_rootitem_vector[i]->text();
                QStringList path_list = job_path.split(":");
                for(auto j = 0; j < frame_info.fileinfo_vector.count(); j ++)
                {
                    QString temp_job_path =  QString::fromStdString(frame_info.fileinfo_vector[j].layout_view->file_name());
                    temp_job_path = temp_job_path.left(temp_job_path.count() - 15);
                    try
                    {
                        if (path_list.count() < 2)
                        {
                            throw "path_list error!";
                        }

                        if (path_list.at(1) == temp_job_path)
                        {
                            is_exits = true;
                        }
                    }
                    catch(...)
                    {
                        logger_widget(QString("job_path error: %1").arg(job_path));
                        return;
                    }
                }

                if (is_exits)
                {
                    QString log = "delete :" + path_list.at(1);
                    int job_id = path_list.at(0).right(path_list.at(0).count() - 3).toInt();
                    int row = m_checklist_model->indexFromItem(m_rootitem_vector.at(i)).row();
                    m_checklist_model->removeRow(row);
                    close_job_key(job_id);
                    emit signal_close_database_widget(job_id);
                    m_rootitem_vector.remove(i);
                }
            }
        }
    }
    else  //the job is delete ahead of time,
    {
        for (auto i = 0; i < m_rootitem_vector.count(); i ++)
        {
            bool is_exits = false;
            QString job_path = m_rootitem_vector[i]->text();
            QStringList path_list = job_path.split(":");
            for(auto j = 0; j < all_filetype_vector.count(); j ++)
            {
                QString temp_job_path =  QString::fromStdString(all_filetype_vector[j].layout_view->file_name());
                temp_job_path = temp_job_path.left(temp_job_path.count() - 15);
                try
                {
                    if (path_list.count() < 2)
                    {
                        throw "path_list error!";
                    }

                    if (path_list.at(1) == temp_job_path)
                    {
                        is_exits = true;
                    }
                }
                catch(...)
                {
                    logger_widget(QString("job_path error: %1").arg(job_path));
                    return;
                }
            }

            if (!is_exits)
            {
                QString log = "delete :" + path_list.at(1);
                logger_file (log);
                int job_id = path_list.at(0).right(path_list.at(0).count() - 3).toInt();
                int row = m_checklist_model->indexFromItem(m_rootitem_vector.at(i)).row();
                m_checklist_model->removeRow(row);
                close_job_key(job_id);
                emit signal_close_database_widget(job_id);
                m_rootitem_vector.remove(i);
            }
        }
    }
}

void CheckList::slot_add_job(const QString& db_path)
{
    logger_file ("add :" + db_path);
    m_jobindex = get_job_key();

    new_read_database(db_path);
    new_update_treeview();

    m_sqlindex_thread->set_database_path(db_path);
    m_sqlindex_thread->start_work();
}

void CheckList::remove_job(int index)
{
    emit signal_close_job(m_active_index_name);
    emit signal_close_database_widget(m_jobindex);
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
    QAction* DeleteAction = new QAction(&Checkmenu);
    QAction* AppendJobAction = new QAction(&Checkmenu);
    DeleteAction->setText("Close Job");
    AppendJobAction->setText("Append Job");

    Checkmenu.addAction(DeleteAction);
    Checkmenu.addAction(AppendJobAction);
    connect(DeleteAction, SIGNAL(triggered()), this, SLOT(slot_close_currentjob()));
    connect(AppendJobAction, SIGNAL(triggered()), this, SLOT(slot_coverage_job()));
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

void CheckList::slot_close_currentjob()
{
    remove_job(m_active_tree_index);
    if (m_checklist_tree->currentIndex().isValid())
        m_checklist_tree->set_active_item(m_checklist_tree->currentIndex());
}

void CheckList::slot_coverage_job()
{
    emit signal_coverage_job();
}

void CheckList::slot_update_job(const FrameInfo & frame_info)
{
}

}
