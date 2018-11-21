#include "choosefiledialog.h"
namespace UI {
ChooseFileDialog::ChooseFileDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *Vlayout = new QVBoxLayout(this);
    QHBoxLayout *Hlayout = new QHBoxLayout();
    gdsFileLabel = new QLabel("GDS file: ", this);
    Hlayout->addWidget(gdsFileLabel);
    Vlayout->addLayout(Hlayout);

    gdsFileEdit = new QLineEdit(this);
    gdsFileButton = new QPushButton("..." ,this);
    Hlayout = new QHBoxLayout();
    Hlayout->addWidget(gdsFileEdit);
    Hlayout->addWidget(gdsFileButton);
    Hlayout->setStretch(0, 8);
    Hlayout->setStretch(1, 1);
    Vlayout->addLayout(Hlayout);

    dataTypeLabel = new QLabel("Datatype: ", this);
    dataTypeEdit = new QLineEdit(this);
    layerLabel = new QLabel("Layer: ", this);
    layerEdit = new QLineEdit(this);
    Hlayout = new QHBoxLayout();
    Hlayout->addWidget(dataTypeLabel);
    Hlayout->addWidget(dataTypeEdit);
    Hlayout->addWidget(layerLabel);
    Hlayout->addWidget(layerEdit);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    gaugeFileLabel = new QLabel("Gauge file: ", this);
    Hlayout->addWidget(gaugeFileLabel);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    gaugeEdit = new QLineEdit(this);
    gaugeButton = new QPushButton("...",this);
    Hlayout->addWidget(gaugeEdit);
    Hlayout->addWidget(gaugeButton);
    Hlayout->setStretch(0, 8);
    Hlayout->setStretch(1, 0);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    okButton = new QPushButton("Ok" ,this);
    cancelButton = new QPushButton("Cancel" ,this);
    Hlayout->addSpacerItem(new QSpacerItem(250, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));
    Hlayout->addWidget(okButton);
    Hlayout->addWidget(cancelButton);
    Vlayout->addLayout(Hlayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}
}
