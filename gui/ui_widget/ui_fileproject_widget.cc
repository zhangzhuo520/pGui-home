#include "ui_fileproject_widget.h"
namespace ui {
FileProjectWidget::FileProjectWidget(QWidget *parent) :
    QWidget(parent)
{
    init();
    setContextMenuPolicy(Qt::CustomContextMenu);
    QVBoxLayout *Vayout = new QVBoxLayout(this);
    Vayout->addWidget(m_project_table);
    Vayout->setContentsMargins(0, 0, 0, 0);
    Vayout->setSpacing(0);
    setLayout(Vayout);
}

void FileProjectWidget::init()
{
    m_project_table = new TableView(this);
    m_project_tablemodel = new FileProjectModel(m_project_table);

    m_project_table->setModel(m_project_tablemodel);
    m_project_table->horizontalHeader()->setClickable(false);
    m_project_table->horizontalHeader()->setStretchLastSection(true);
    m_project_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    m_project_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_project_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_project_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_project_table->verticalHeader()->hide();

    connect(m_project_table, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_DoubleClickItem(QModelIndex)));
    connect(m_project_table, SIGNAL(clicked(QModelIndex)), this, SLOT(slot_ClickItem(QModelIndex)));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slot_context_menu(QPoint)));
}

bool FileProjectWidget::is_file_exist(QString filename)
{
    return m_project_tablemodel->find_file(filename);
}

void FileProjectWidget::delete_file(QString filename)
{
    emit signal_close_currentFile(filename);
    m_project_tablemodel->delete_File(filename);
}

void FileProjectWidget::slot_DoubleClickItem(QModelIndex index)
{
    m_active_index = index.row();
    m_active_filename = index.data().toString();
    emit signal_create_canvas(index);
}

void FileProjectWidget::slot_ClickItem(QModelIndex modelIndex)
{
    m_active_filename = modelIndex.data().toString();
    m_active_index = modelIndex.row();
}

void FileProjectWidget::slot_context_menu(QPoint pos)
{
    QMenu *context_menu = new QMenu(this);
    QAction *close_item_action = new QAction("close file", context_menu);
    QAction *open_file_action = new QAction("open file", context_menu);
    QAction *append_file_action = new QAction("append this file to ...", context_menu);
    QAction *detach_file_action = new QAction("detach file", context_menu);
    context_menu->addAction(open_file_action);
    context_menu->addAction(close_item_action);
    context_menu->addAction(append_file_action);
    context_menu->addAction(detach_file_action);

    render::LayoutView* lv = *(m_project_tablemodel->get_layout_view_iter(m_active_index));
    if(!lv->enable_attach())
    {
        append_file_action->setEnabled(false);
    }
    connect(close_item_action, SIGNAL(triggered()), this, SLOT(slot_CloseItem()));
    connect(open_file_action, SIGNAL(triggered()), this, SLOT(slot_OpenFile()));
    connect(append_file_action, SIGNAL(triggered()),  this, SLOT(slot_AppendFile()));
    connect(detach_file_action, SIGNAL(triggered()), this, SLOT(slot_DetachFile()));
    QPoint tempPos = pos;
    tempPos.setY(tempPos.y());
    context_menu->exec(m_project_table->mapToGlobal(tempPos));

}

void FileProjectWidget::slot_AppendFile()
{
    int rowCount = m_project_tablemodel->rowCount(QModelIndex());
    if(rowCount == 1)
    {
        showWarning(this, "Warning", "Only one file exists in the file manager.");
    }

    if(m_select_file_dialog == NULL)
    {
        m_select_file_dialog = new QDialog(this);
    }

    m_select_file_dialog->setWindowTitle("Append file");
    m_file_label = new QLabel("Select file:", m_select_file_dialog);

    m_file_box = new QComboBox(m_select_file_dialog);
    QFont font;
    QFontMetrics fm(font);
    int maxWidth = 0;

    m_file_box->setGeometry(30, 60, 250, 30);
    for(int i = 0; i < rowCount; i++)
    {
        render::LayoutView* lv = * (m_project_tablemodel->get_layout_view_iter(i));
        QString filename = QString::fromStdString(lv->file_name());
        if(m_active_filename != filename)
        {
            QList<QVariant> list;
            list.append(QVariant(i));
            list.append(QVariant(m_active_index));
            int width = fm.width(filename);
            if(maxWidth < width)
            {
                maxWidth = width;
            }
            m_file_box->addItem(QString::fromStdString(lv->file_name()), QVariant(list));
        }
    }

    m_select_file_okbutton = new QPushButton("OK", m_select_file_dialog);
    connect(m_select_file_okbutton, SIGNAL(clicked()), this, SLOT(slot_SelectAppendFile()));
    m_select_file_cancelbutton = new QPushButton("Cancel", m_select_file_dialog);
    connect(m_select_file_cancelbutton, SIGNAL(clicked()), m_select_file_dialog, SLOT(close()));

    maxWidth += 50;
    m_file_box->setGeometry(30, 60, maxWidth < 150 ? 150 : maxWidth , 30);
    m_select_file_dialog->setGeometry(width() / 2 , height() / 2 , maxWidth > 300 ? maxWidth : 300, 140);
    m_file_label->setGeometry(30, 30, 70, 25);
    m_select_file_okbutton->setGeometry(150, 100, 60, 30);
    m_select_file_cancelbutton->setGeometry(220, 100, 60, 30);

    m_select_file_dialog->show();
}

void FileProjectWidget::slot_SelectAppendFile()
{
    m_select_file_dialog->close();
    int index = m_file_box->currentIndex();
    QVariant var = m_file_box->itemData(index);
    QList<QVariant> list = var.toList();
    QVariant to = list.at(0);
    QVariant from = list.at(1);
    int to_index = to.toInt();
    int from_index = from.toInt();

    render::LayoutView* to_lv = *(m_project_tablemodel->get_layout_view_iter(to_index));
    render::LayoutView* from_lv = *(m_project_tablemodel->get_layout_view_iter(from_index));

    QModelIndex modelIndex = m_project_tablemodel->index(from_index, 0);

    if(to_lv->get_widget() == 0)
    {
        if (showWarning(this, "Warning", "The selected file does not have frame.Do you want re-open it", QMessageBox::StandardButtons(QMessageBox::No | QMessageBox::Ok))
                == QMessageBox::Ok )
        {
            m_project_table->double_click_item(modelIndex);
        }
        return;
    }

    from_lv->detach();
    from_lv->attach(to_lv->get_widget(), "" , true);
}

void FileProjectWidget::slot_DetachFile()
{
    layout_view_iter it = m_project_tablemodel->get_layout_view_iter(m_active_index);
    (*it)->detach();
}

void FileProjectWidget::slot_addFile(QString path, bool isOverLay)
{
    render::LayoutView* lv = new render::LayoutView();
    std::string s = path.toStdString();
    lv->set_file_name(s);
    m_project_tablemodel->insertRow(m_project_tablemodel->rowCount(QModelIndex()), lv);
    QModelIndex modelIndex = m_project_tablemodel->index(m_project_tablemodel->rowCount(QModelIndex()) - 1, 0);
    if (isOverLay)
    {
        TIME_DEBUG
        emit signal_create_overlay_canvas(modelIndex);
    }
    else
    {
        m_project_table->double_click_item(modelIndex);
    }
}

void FileProjectWidget::slot_CloseItem()
{
    if (m_active_index < 0 || m_active_index > (m_project_tablemodel->rowCount(QModelIndex()) - 1))
    {
        showWarning(this, "Warning", "Not Select File!");
        return;
    }

//    layout_view_iter it = m_project_tablemodel->get_layout_view_iter(m_active_index);
//    if(((*it)->get_widget() == 0) || ((*it)->get_widget()->layout_views_size() == 1))
//    {
//        emit signal_close_currentFile(m_active_filename);
//    }

    m_project_tablemodel->removeFile(m_active_index);

    //there are 3 item, close first item, new current item row is 1, should be 0, so click.
    if (m_project_table->currentIndex().isValid())
    {
        m_project_table->set_active_item(m_project_table->currentIndex());
    }
}

void FileProjectWidget::slot_OpenFile()
{
    emit signal_openFile();
}

}
