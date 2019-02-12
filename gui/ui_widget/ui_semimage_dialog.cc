#include "ui_semimage_dialog.h"
namespace ui {

SemImageDialog::SemImageDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout * Vlayout = new QVBoxLayout(this);
    QHBoxLayout * Hlayout = new QHBoxLayout();
    fileLabel = new QLabel("Flie:", this);
    Hlayout->addWidget(fileLabel);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    fileEdit = new QLineEdit(this);
    fileButton = new QPushButton("..." ,this);
    Hlayout->addWidget(fileEdit);
    Hlayout->addWidget(fileButton);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    moveCheckBox = new QCheckBox("Move to paste location", this);
    dismissCheckBox = new QCheckBox("Dismiss dialog after paste", this);
    Hlayout->addSpacerItem(new QSpacerItem(100, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));
    Hlayout->addWidget(moveCheckBox);
    Hlayout->addWidget(dismissCheckBox);
    Vlayout->addLayout(Hlayout);

    QVBoxLayout * VleftLayout = new QVBoxLayout();
    VleftLayout->setContentsMargins(0, 0, 0, 0);
    Hlayout = new QHBoxLayout();
    widthLabel = new QLabel("Width:",this);
    widthLabel->setFixedWidth(50);
    widthEdit = new QLineEdit(this);
    Hlayout->addWidget(widthLabel);
    Hlayout->addWidget(widthEdit);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 8);
    VleftLayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    heightLabel = new QLabel("Height:",this);
    heightLabel->setFixedWidth(50);
    heightEdit = new QLineEdit(this);
    Hlayout->addWidget(heightLabel);
    Hlayout->addWidget(heightEdit);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 8);
    VleftLayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    centerLabel = new QLabel("Center X:", this);
    centerLabel->setFixedWidth(50);
    centerXEdit = new QLineEdit(this);
    yLabel = new QLabel("Y:",this);
    yEdit = new QLineEdit(this);

    Hlayout->addWidget(centerLabel);
    Hlayout->addWidget(centerXEdit);
    Hlayout->addWidget(yLabel);
    Hlayout->addWidget(yEdit);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 8);
    Hlayout->setStretch(2, 1);
    Hlayout->setStretch(3, 8);
    VleftLayout->addLayout(Hlayout);

    QVBoxLayout *VrightLayout = new QVBoxLayout();
    VrightLayout->setContentsMargins(0, 0, 0, 0);
    okButton = new QPushButton("Ok", this);
    cancelButton = new QPushButton("Cancel", this);
    VrightLayout->addSpacerItem(new QSpacerItem(10, 40, QSizePolicy::Preferred, QSizePolicy::Preferred));
    VrightLayout->addWidget(okButton);
    VrightLayout->addWidget(cancelButton);

    Hlayout = new QHBoxLayout();
    Hlayout->addLayout(VleftLayout);
    Hlayout->addLayout(VrightLayout);
    Hlayout->setStretch(0, 8);
    Hlayout->setStretch(1, 1);
    Vlayout->addLayout(Hlayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}
}
