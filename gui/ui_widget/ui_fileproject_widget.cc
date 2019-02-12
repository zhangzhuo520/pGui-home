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
    emit signal_creat_canves(index);
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
    context_menu->addAction(open_file_action);
    context_menu->addAction(close_item_action);
    connect(close_item_action, SIGNAL(triggered()), this, SLOT(slot_CloseItem()));
    connect(open_file_action, SIGNAL(triggered()), this, SLOT(slot_OpenFile()));
    QPoint tempPos = pos;
    tempPos.setY(tempPos.y());
    context_menu->exec(m_project_table->mapToGlobal(tempPos));

}

void FileProjectWidget::slot_addFile(QString path, bool isOverLay)
{
    render::LayoutView lv;
    std::string s = path.toStdString();
    lv.set_file_name(s);
    m_project_tablemodel->insertRow(m_project_tablemodel->rowCount(QModelIndex()), lv);
    QModelIndex modelIndex = m_project_tablemodel->index(m_project_tablemodel->rowCount(QModelIndex()) - 1, 0);
    if (isOverLay)
    {
        TIME_DEBUG
        emit signal_creat_overlay_canves(modelIndex);
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

    FileProjectModel::layout_view_iter it = m_project_tablemodel->get_layout_view_iter(m_active_index);
    if( it->get_widget()->layout_views_size() == 1)
    {
        emit signal_close_currentFile(m_active_filename);
    }
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
