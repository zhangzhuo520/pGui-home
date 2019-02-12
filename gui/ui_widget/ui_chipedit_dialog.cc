#include "ui_chipedit_dialog.h"
namespace ui {
chipEditDialog::chipEditDialog(QWidget *parent) :
    QDialog(parent)
{
    QHBoxLayout *Hlayout = new QHBoxLayout(this);
    QVBoxLayout *VleftLayout = new QVBoxLayout();

    QHBoxLayout *HleftLayout = new QHBoxLayout();
    HleftLayout->setContentsMargins(0, 0, 0, 0);
    HleftLayout->setSpacing(2);
    m_config_label = new QLabel("Configuration: ", this);
    m_config_edit = new QLineEdit(this);
    HleftLayout->addWidget(m_config_label);
    HleftLayout->addWidget(m_config_edit);
    HleftLayout->setStretch(0, 2);
    HleftLayout->setStretch(1, 18);
    VleftLayout->addLayout(HleftLayout);

    m_message_label = new QLabel(this);
    HleftLayout = new QHBoxLayout();
    HleftLayout->addWidget(m_message_label);
    VleftLayout->addLayout(HleftLayout);

    m_config_textedit = new QTextEdit(this);
    VleftLayout->addWidget(m_config_textedit);

    QVBoxLayout *VrightLayout = new QVBoxLayout();
    VrightLayout->setSpacing(2);
    VrightLayout->setContentsMargins(0, 0, 0, 0);
    m_saveas_button = new QPushButton("Save As", this);
    m_save_button = new QPushButton("Save",this);
    m_open_button = new QPushButton("Open",this);
    m_forma_button = new QPushButton("Format",this);
    m_preview_button = new QPushButton("Preview",this);
    m_generate_button = new QPushButton("Generate",this);
    m_close_button = new QPushButton("Close",this);

    VrightLayout->addWidget(m_open_button);
    VrightLayout->addWidget(m_save_button);
    VrightLayout->addWidget(m_saveas_button);
    VrightLayout->addWidget(m_forma_button);
    VrightLayout->addWidget(m_preview_button);
    VrightLayout->addWidget(m_generate_button);
    VrightLayout->addSpacerItem(new QSpacerItem(10, 500, QSizePolicy::Preferred, QSizePolicy::Preferred));
    VrightLayout->addWidget(m_close_button);

    Hlayout->addLayout(VleftLayout);
    Hlayout->addLayout(VrightLayout);

    connect(m_close_button, SIGNAL(clicked()), this, SLOT(close()));
}
}
