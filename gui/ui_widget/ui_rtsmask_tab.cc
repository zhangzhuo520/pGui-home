#include "ui_rtsmask_tab.h"
namespace ui {

RtsMaskTab::RtsMaskTab(QWidget *parent):
    QTabWidget(parent)
{
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

MaskWidget::MaskWidget(QWidget *parent):
    QWidget(parent)
{
      init_ui();
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
    m_layer_table = new QTableWidget(this);
    HCenterLayout->addWidget(m_layer_table);
    QHBoxLayout *HbottomLayout = new QHBoxLayout();
    m_boolean_label = new QLabel("Boolean Operation: " ,this);
    m_boolean_edit = new QLineEdit(this);
    HbottomLayout->addWidget(m_boolean_label);
    HbottomLayout->addWidget(m_boolean_edit);
    QVBoxLayout *VLayout = new QVBoxLayout(this);
    VLayout->addLayout(HToplayout);
    VLayout->addLayout(HCenterLayout);
    VLayout->addLayout(HbottomLayout);
    setLayout(VLayout);
}
}
