#include "ui_rtsmask_tab.h"
namespace ui {
RtsMaskTab::RtsMaskTab(QWidget *parent):
    QTabWidget(parent)
{
    tabBar()->setStyle(new CustomTabStyle);
}

RtsMaskTab::~RtsMaskTab()
{
}

void RtsMaskTab::init_tab(const QStringList & Tablist)
{
    delete_all_tab();
    for (int i = 0; i < Tablist.count(); i ++)
    {
        m_mask_widget = new MaskWidget(this);
        m_mask_widget->set_layername_list(m_layername_list);
        addTab(m_mask_widget, Tablist.at(i));
    }
}

void RtsMaskTab::delete_all_tab()
{
    int TabCount = count();
    for (int i = 0; i < TabCount; i ++)
    {
        delete widget(0);
        removeTab(0);
    }
}

void RtsMaskTab::set_layername_list(const QStringList & list)
{
    m_layername_list = list;
}

MaskWidget::MaskWidget(QWidget *parent):
    QWidget(parent)
{
      init_ui();
      init_tabwidget();
      init_connection();
}

MaskWidget::~MaskWidget()
{
}

void MaskWidget::init_ui()
{
    QHBoxLayout *HToplayout = new QHBoxLayout();
    m_mask_add_button = new QPushButton("Add", this);
    m_mask_clone_button = new QPushButton("Clone", this);
    m_mask_delete_button = new QPushButton("Delete", this);
    HToplayout->addWidget(m_mask_add_button);
    HToplayout->addWidget(m_mask_clone_button);
    HToplayout->addWidget(m_mask_delete_button);
    QHBoxLayout *HCenterLayout = new QHBoxLayout();
    m_layer_table = new QTableView(this);
    HCenterLayout->addWidget(m_layer_table);
    QHBoxLayout *HbottomLayout = new QHBoxLayout();
    m_boolean_label = new QLabel("Boolean Operation: " ,this);
    m_boolean_edit = new QLineEdit(this);
    HbottomLayout->addWidget(m_boolean_label);
    HbottomLayout->addWidget(m_boolean_edit);
    QVBoxLayout *VLayout = new QVBoxLayout(this);
    VLayout->setSpacing(0);
    VLayout->setContentsMargins(0, 0, 0, 0);
    VLayout->addLayout(HToplayout);
    VLayout->addLayout(HCenterLayout);
    VLayout->addLayout(HbottomLayout);
    setLayout(VLayout);
}

void MaskWidget::init_tabwidget()
{
    m_mask_model = new RtsMaskModel(m_layer_table);
    m_layer_table->setModel(m_mask_model);
    m_layer_table->setItemDelegate(new RtsMaskDelegate());
    m_layer_table->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_layer_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_layer_table->setEditTriggers(QAbstractItemView::SelectedClicked);
    m_layer_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_layer_table->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    m_layer_table->setColumnWidth(0, 50);
}

void MaskWidget::init_connection()
{
    connect(m_mask_delete_button, SIGNAL(clicked()), this, SLOT(slot_delete_row()));
    connect(m_mask_add_button, SIGNAL(clicked()), this, SLOT(slot_add_row()));
    connect(m_mask_clone_button, SIGNAL(clicked()), this, SLOT(slot_clone_row()));
}

void MaskWidget::set_layername_list(const QStringList & list)
{
    m_layername_list = list;
}

void MaskWidget::slot_delete_row()
{
}

void MaskWidget::slot_add_row()
{
    if (m_layername_list.isEmpty())
    {
        return;
    }
    m_mask_model->append_row(m_layername_list.at(0));
}

void MaskWidget::slot_clone_row()
{
}
}
