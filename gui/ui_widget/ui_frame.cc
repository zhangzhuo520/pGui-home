#include "ui_frame.h"
namespace ui {
Frame::Frame(QFrame *parent) :
    QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setObjectName("Frame");
    setMouseTracking(true);
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout = new QHBoxLayout();
    initTitleBar();

    mianwindow = new MainWindow(this);
    mianwindow->setObjectName("Mainwindow");
    TitleBar->setObjectName("TitleBar");

    hLayout->addWidget(signLable);
    hLayout->addWidget(spaceLabel);
    hLayout->addWidget(MinButton);
    hLayout->addWidget(MaxButton);
    hLayout->addWidget(CloseButton);

    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->setStretch(0, 1);
    hLayout->setStretch(1, 20);
    hLayout->setStretch(2, 1);
    hLayout->setStretch(3, 1);
    hLayout->setStretch(4, 1);

    TitleBar->setLayout(hLayout);
    vLayout->addWidget(TitleBar);
    vLayout->addWidget(mianwindow);
    setLayout(vLayout);
    vLayout->setMargin(2);
    vLayout->setSpacing(0);
    connect(mianwindow, SIGNAL(signal_close()), this, SLOT(close()));
    setWindowsSizeChange();
}

void Frame::initTitleBar()
{
    TitleBar = new QWidget(this);
    TitleBar->setGeometry(0, 0, width(), 20);
    TitleBar->setMaximumHeight(23);
    signLable = new QLabel(TitleBar);
    QPixmap pixmap(":/dfjy/images/dfjy.png");
    pixmap = pixmap.scaled(QSize(50, 23), Qt::KeepAspectRatio);
    signLable->setPixmap(pixmap);
    spaceLabel = new QLabel(TitleBar);
    spaceLabel->setText("    PanGenGUI V1.0.0.1");
    spaceLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter );
    MaxButton = new PushButton(TitleBar);
    MinButton = new PushButton(TitleBar);
    CloseButton = new PushButton(TitleBar);
    MinButton->setIcon(QIcon(":/dfjy/images/min.png"));
    CloseButton->setIcon(QIcon(":/dfjy/images/close.png"));
    MaxButton->setIcon(QIcon(":/dfjy/images/max.png"));
    connect(MaxButton, SIGNAL(clicked()), this, SLOT(slot_changeWindowSize()));
    connect(MinButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(CloseButton, SIGNAL(clicked()), this, SLOT(close()));
}

void Frame::setWindowsSizeChange()
{
    pHelper = new FramelessHelper(this);
    pHelper->activateOn(this);
    pHelper->setTitleHeight(30);
    pHelper->setWidgetMovable(true);
    pHelper->setWidgetResizable(true);
    pHelper->setRubberBandOnMove(true);
    pHelper->setRubberBandOnResize(true);
}

void Frame::slot_changeWindowSize()
{
    if (!isMaximized())
    {
        MaxButton->setIcon(QIcon(":/dfjy/images/normal.png"));
        showMaximized();
    }
    else
    {
        MaxButton->setIcon(QIcon(":/dfjy/images/max.png"));
        showNormal();
    }

}

}
