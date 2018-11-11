#include "chipeditdialog.h"
namespace UI {
chipEditDialog::chipEditDialog(QWidget *parent) :
    QDialog(parent)
{
    QHBoxLayout *Hlayout = new QHBoxLayout(this);
    QVBoxLayout *VleftLayout = new QVBoxLayout();

    QHBoxLayout *HleftLayout = new QHBoxLayout();
    HleftLayout->setContentsMargins(0, 0, 0, 0);
    HleftLayout->setSpacing(2);
    configLabel = new QLabel("Configuration: ", this);
    configEdit = new QLineEdit(this);
    HleftLayout->addWidget(configLabel);
    HleftLayout->addWidget(configEdit);
    HleftLayout->setStretch(0, 2);
    HleftLayout->setStretch(1, 18);
    VleftLayout->addLayout(HleftLayout);

    showMessageLabel = new QLabel(this);
    HleftLayout = new QHBoxLayout();
    HleftLayout->addWidget(showMessageLabel);
    VleftLayout->addLayout(HleftLayout);

    configTextEdit = new QTextEdit(this);
    VleftLayout->addWidget(configTextEdit);

    QVBoxLayout *VrightLayout = new QVBoxLayout();
    VrightLayout->setSpacing(2);
    VrightLayout->setContentsMargins(0, 0, 0, 0);
    saveasButton = new QPushButton("Save As", this);
    saveButton = new QPushButton("Save",this);
    openButton = new QPushButton("Open",this);
    formatButton = new QPushButton("Format",this);
    previewButton = new QPushButton("Preview",this);
    generateButton = new QPushButton("Generate",this);
    closeButton = new QPushButton("Close",this);

    VrightLayout->addWidget(openButton);
    VrightLayout->addWidget(saveButton);
    VrightLayout->addWidget(saveasButton);
    VrightLayout->addWidget(formatButton);
    VrightLayout->addWidget(previewButton);
    VrightLayout->addWidget(generateButton);
    VrightLayout->addSpacerItem(new QSpacerItem(10, 500, QSizePolicy::Preferred, QSizePolicy::Preferred));
    VrightLayout->addWidget(closeButton);

    Hlayout->addLayout(VleftLayout);
    Hlayout->addLayout(VrightLayout);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}
}
