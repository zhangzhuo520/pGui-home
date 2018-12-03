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
    m_project_table = new QTableView(this);
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

void FileProjectWidget::slot_DoubleClickItem(QModelIndex index)
{
    emit signal_DoubleClickItem(index);
}

void FileProjectWidget::slot_ClickItem(QModelIndex modelIndex)
{
    m_current_index = modelIndex.row();
    emit signal_DoubleClickItem(modelIndex);
}

void FileProjectWidget::slot_context_menu(QPoint pos)
{
    QMenu *context_menu = new QMenu(this);
    QAction *close_item_action = new QAction("close file", context_menu);
    context_menu->addAction(close_item_action);
    connect(close_item_action, SIGNAL(triggered()), this, SLOT(slot_CloseItem()));
    QPoint tempPos = pos;
    tempPos.setY(tempPos.y() + 22);
    context_menu->exec(m_project_table->mapToGlobal(tempPos));

}

void FileProjectWidget::slot_addFile(QString path)
{
    render::LayoutView lv;
    std::string s = path.toStdString();
    lv.set_file_name(s);
    m_project_tablemodel->insertRow(m_project_tablemodel->rowCount(QModelIndex()), lv);
}

void FileProjectWidget::slot_CloseItem()
{
    m_project_tablemodel->removeRow(m_current_index);
}
}
