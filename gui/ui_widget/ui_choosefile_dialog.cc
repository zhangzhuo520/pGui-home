#include "ui_choosefile_dialog.h"
namespace ui {
ChooseFileDialog::ChooseFileDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *Vlayout = new QVBoxLayout(this);
    QHBoxLayout *Hlayout = new QHBoxLayout();
    m_gdsfile_label = new QLabel("GDS file: ", this);
    Hlayout->addWidget(m_gdsfile_label);
    Vlayout->addLayout(Hlayout);

    m_gdsfile_edit = new QLineEdit(this);
    m_gdsfile_button = new QPushButton("..." ,this);
    Hlayout = new QHBoxLayout();
    Hlayout->addWidget(m_gdsfile_edit);
    Hlayout->addWidget(m_gdsfile_button);
    Hlayout->setStretch(0, 8);
    Hlayout->setStretch(1, 1);
    Vlayout->addLayout(Hlayout);

    m_datatype_label = new QLabel("Datatype: ", this);
    dataTypeEdit = new QLineEdit(this);
    m_layer_label = new QLabel("Layer: ", this);
    m_layer_edit = new QLineEdit(this);
    Hlayout = new QHBoxLayout();
    Hlayout->addWidget(m_datatype_label);
    Hlayout->addWidget(dataTypeEdit);
    Hlayout->addWidget(m_layer_label);
    Hlayout->addWidget(m_layer_edit);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    m_gaugfile_label = new QLabel("Gauge file: ", this);
    Hlayout->addWidget(m_gaugfile_label);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    m_gaugfile_edit = new QLineEdit(this);
    m_gaugfile_button = new QPushButton("...",this);
    Hlayout->addWidget(m_gaugfile_edit);
    Hlayout->addWidget(m_gaugfile_button);
    Hlayout->setStretch(0, 8);
    Hlayout->setStretch(1, 0);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    m_ok_button = new QPushButton("Ok" ,this);
    m_cancel_button = new QPushButton("Cancel" ,this);
    Hlayout->addSpacerItem(new QSpacerItem(250, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));
    Hlayout->addWidget(m_ok_button);
    Hlayout->addWidget(m_cancel_button);
    Vlayout->addLayout(Hlayout);

    connect(m_cancel_button, SIGNAL(clicked()), this, SLOT(close()));
}
}
