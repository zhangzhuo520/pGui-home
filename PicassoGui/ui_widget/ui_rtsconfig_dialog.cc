#include "ui_rtsconfig_dialog.h"
namespace ui {

RtsConfigDialog::RtsConfigDialog(QWidget *parent) :
    QDialog(parent)
{
    initTopButton();
    initRtsWidget();
    initTabWidget();
    initBottomButton();
    Vlayout = new QVBoxLayout(this);
    Hlayout = new QHBoxLayout;
    Hlayout->setSpacing(10);
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->addWidget(addRtsButton);
    Hlayout->addWidget(deleteRtsButton);
    Hlayout->addWidget(reorderButton);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    Hlayout->addWidget(rtsTabWidget);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    Hlayout->addSpacerItem(new QSpacerItem(200, 20, QSizePolicy::Preferred, QSizePolicy::Preferred));
    Hlayout->addWidget(okButton);
    Hlayout->addWidget(cancelButton);
    Hlayout->addWidget(applyButton);
    Vlayout->addLayout(Hlayout);

    setLayout(Vlayout);
}

RtsConfigDialog::~RtsConfigDialog()
{
}

void RtsConfigDialog::initRtsWidget()
{
    rtsWidget = new QWidget(this);
    tabVlayout = new QVBoxLayout();


    Hlayout = new QHBoxLayout();
    QLabel *label1 = new QLabel("Model", rtsWidget);
    label1->setFixedWidth(LableTitilWidth);
    modelCommbox = new Commbox(rtsWidget);
    modelButton = new QPushButton("...",rtsWidget);
    modelButton->setFixedWidth(pushButtonWidth);
    Hlayout->addWidget(label1);
    Hlayout->addWidget(modelCommbox);
    Hlayout->addWidget(modelButton);
    Hlayout->setStretch(0, 2);
    Hlayout->setStretch(1, 9);
    Hlayout->setStretch(2, 1);
    tabVlayout->addLayout(Hlayout);

    QVBoxLayout *mVlayout = new QVBoxLayout();
    Hlayout = new QHBoxLayout();
    QLabel *label2 = new QLabel("Mask", rtsWidget);
    label2->setFixedWidth(LableTitilWidth);
    mVlayout->addWidget(label2);
    label2 = new QLabel("  ", rtsWidget);
    label2->setMinimumWidth(LableWidth);
    mVlayout->addWidget(label2);
    mVlayout->setStretch(0, 1);
    mVlayout->setStretch(1, 5);
    Hlayout->addLayout(mVlayout);

    maskFrame = new QFrame(rtsWidget);
    QPalette pale = maskFrame->palette();
    pale.setBrush(QPalette::Window, Qt::black);
    maskFrame->setPalette(pale);
    Hlayout->addWidget(maskFrame);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 5);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label3 = new QLabel("General", rtsWidget);
    label3->setFixedWidth(LableTitilWidth);
    Hlayout->addWidget(label3);
    label3 = new QLabel("Mask Bias(nm):     ", rtsWidget);
    label3->setMinimumWidth(LableWidth);
    Hlayout->addWidget(label3);
    maskBiasEidt = new QLineEdit(rtsWidget);
    Hlayout->addWidget(maskBiasEidt);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 1);
    Hlayout->setStretch(2, 8);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label4 = new QLabel("Options", rtsWidget);
    label4->setFixedWidth(LableTitilWidth);
    Hlayout->addWidget(label4);
    label4 = new QLabel("Delta Dose(%):  ", rtsWidget);
    label4->setMinimumWidth(LableWidth);
    Hlayout->addWidget(label4);
    DeltaDoseEdit  = new QLineEdit(rtsWidget);
    Hlayout->addWidget(DeltaDoseEdit);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 1);
    Hlayout->setStretch(2, 8);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label5 = new QLabel("  ", rtsWidget);
    label5->setFixedWidth(LableTitilWidth);
    Hlayout->addWidget(label5);
    label5 = new QLabel("Defocus:    ", rtsWidget);
    label5->setMinimumWidth(LableWidth);
    Hlayout->addWidget(label5);
    defocusCommbox = new Commbox(rtsWidget);
    Hlayout->addWidget(defocusCommbox);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 1);
    Hlayout->setStretch(2, 8);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label6 = new QLabel("           ", rtsWidget);
    Hlayout->addWidget(label6);
    label6->setFixedWidth(LableTitilWidth);
    label6 = new QLabel("Working Directory: ", rtsWidget);
    Hlayout->addWidget(label6);
    label6->setMinimumWidth(LableWidth);
    WorkingEdit = new QLineEdit(rtsWidget);
    workButton = new QPushButton("...",rtsWidget);
    workButton->setFixedWidth(pushButtonWidth);
    QHBoxLayout *HworkLayout = new QHBoxLayout();
    HworkLayout->addWidget(WorkingEdit);
    HworkLayout->addWidget(workButton);
    HworkLayout->setStretch(0, 5);
    HworkLayout->setStretch(1, 1);
    Hlayout->addLayout(HworkLayout);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 1);
    Hlayout->setStretch(2, 8);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label7 = new QLabel("  ", rtsWidget);
    Hlayout->addWidget(label7);
    label7->setFixedWidth(LableTitilWidth);
    label7 = new QLabel("xCorrection Binary:", rtsWidget);
    Hlayout->addWidget(label7);
    label7->setMinimumWidth(LableWidth);
    xCorrectionEdit = new QLineEdit(rtsWidget);
    Hlayout->addWidget(xCorrectionEdit);
    xCorrectionButton = new QPushButton("...",rtsWidget);
    xCorrectionButton->setFixedWidth(pushButtonWidth);
    Hlayout->addWidget(xCorrectionButton);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label8 = new QLabel("     ", rtsWidget);
    Hlayout->addWidget(label8);
    label8->setFixedWidth(LableTitilWidth);
    useGPUcheckbox = new QCheckBox(rtsWidget);
    useGPUcheckbox->setText("Use GPU");
    useGPUcheckbox->setMinimumWidth(LableWidth);
    Hlayout->addWidget(useGPUcheckbox);
    label8 = new QLabel(" ", rtsWidget);
    label8->setMinimumWidth(200);
    Hlayout->addWidget(label8);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label19 = new QLabel(" ", rtsWidget);
    label19->setFixedWidth(LableTitilWidth);
    QFrame *line = new QFrame(rtsWidget);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    Hlayout->addWidget(label19);
    Hlayout->addWidget(line);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 9);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label20 = new QLabel("EUV", rtsWidget);
    label20->setFixedWidth(LableTitilWidth);
    Hlayout->addWidget(label20);
    mapRadioButton = new QRadioButton("Flar Map", rtsWidget);
    mapRadioButton->setMinimumWidth(LableWidth);
    Hlayout->addWidget(mapRadioButton);
    flareEdit = new QLineEdit(rtsWidget);
    FlareButton = new QPushButton("...",rtsWidget);
    ViewButton = new QPushButton("View",rtsWidget);

    FlareButton->setFixedWidth(pushButtonWidth);
    ViewButton->setFixedWidth(pushButtonWidth);
    Hlayout->addWidget(flareEdit);
    Hlayout->addWidget(FlareButton);
    Hlayout->addWidget(ViewButton);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 1);
    Hlayout->setStretch(2, 8);
    Hlayout->setStretch(3, 1);
    Hlayout->setStretch(4, 1);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *label21 = new QLabel("Options", rtsWidget);
    label21->setFixedWidth(LableTitilWidth);
    Hlayout->addWidget(label21);

    QLabel *label22 = new QLabel("Map Offset(x, y)", rtsWidget);
    label22->setFixedWidth(LableWidth);
    Hlayout->addWidget(label22);

    mapOffsetEdit = new QLineEdit(rtsWidget);
    Hlayout->addWidget(mapOffsetEdit);

    QLabel *label23 = new QLabel("(um)", rtsWidget);
    label23->setFixedWidth(LableTitilWidth);
    Hlayout->addWidget(label23);

    QLabel *label24 = new QLabel("(Sampling Ratio)", rtsWidget);
    Hlayout->addWidget(label24);

    sampSpinBox = new QSpinBox(rtsWidget);
    Hlayout->addWidget(sampSpinBox);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *labe25 = new QLabel("  ", rtsWidget);
    Hlayout->addWidget(labe25);
    labe25->setFixedWidth(LableTitilWidth);
    constantRadioButton = new QRadioButton("Constant Flare:", rtsWidget);
    constantRadioButton->setFixedWidth(LableWidth);
    Hlayout->addWidget(constantRadioButton);

    constantEdit = new QLineEdit(rtsWidget);
    Hlayout->addWidget(constantEdit);

    labe25 = new QLabel("%flare", rtsWidget);
    Hlayout->addWidget(labe25);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *labe26 = new QLabel("  ", rtsWidget);
    labe26->setFixedWidth(LableTitilWidth);
    Hlayout->addWidget(labe26);
    labe26 = new QLabel("Slit Model:    ", rtsWidget);
    labe26->setMinimumWidth(LableWidth);
    Hlayout->addWidget(labe26);
    modelCommbox = new Commbox(rtsWidget);
    Hlayout->addWidget(modelCommbox);
    Hlayout->setStretch(0, 1);
    Hlayout->setStretch(1, 1);
    Hlayout->setStretch(2, 8);
    tabVlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    QLabel *labe27 = new QLabel("     ", rtsWidget);
    Hlayout->addWidget(labe27);
    labe27->setFixedWidth(LableTitilWidth);
    berderCheckbox = new QCheckBox(rtsWidget);
    berderCheckbox->setText("Black Border");
    berderCheckbox->setMinimumWidth(LableWidth);
    Hlayout->addWidget(berderCheckbox);
    labe27 = new QLabel(" ", rtsWidget);
    labe27->setMinimumWidth(200);
    Hlayout->addWidget(labe27);
    tabVlayout->addLayout(Hlayout);

    rtsWidget->setLayout(tabVlayout);
}

void RtsConfigDialog::initTopButton()
{
    addRtsButton = new QPushButton("Add TRS", this);
    deleteRtsButton = new QPushButton("Delete TRS",this);
    reorderButton = new QPushButton("Reorder", this);
    connect(addRtsButton, SIGNAL(clicked()), this, SLOT(slotsAddRts()));
    connect(deleteRtsButton, SIGNAL(clicked()), this, SLOT(slotsDeleteRts()));
    connect(reorderButton, SIGNAL(clicked()), this, SLOT(slotsReorder()));
}

void RtsConfigDialog::initTabWidget()
{
    rtsTabWidget = new QTabWidget(this);
    QString tabTitle = "rts_" + QString::number(1);
    rtsTabWidget->addTab(rtsWidget, tabTitle);
}

void RtsConfigDialog::initBottomButton()
{
    okButton = new QPushButton("Ok", this);
    cancelButton = new QPushButton("Cancel",this);
    applyButton = new QPushButton("Apply",this);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

void RtsConfigDialog::slotsAddRts()
{
    initRtsWidget();
    QString tabTitle = "rts_" + QString::number(rtsTabWidget->count() + 1);
    rtsTabWidget->addTab(rtsWidget, tabTitle);
}

void RtsConfigDialog::slotsDeleteRts()
{
    rtsTabWidget->removeTab(rtsTabWidget->count() - 1);
}

void RtsConfigDialog::slotsReorder()
{
}
}
