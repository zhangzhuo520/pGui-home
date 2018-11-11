#include "layerwidget.h"

LayerWidget::LayerWidget(QWidget *parent) :
    QWidget(parent)
{

    initToolBar();
    initTree();
    QVBoxLayout *Vayout = new QVBoxLayout(this);

    Vayout->addWidget(layerToolBar);
    Vayout->addLayout(TreeHLayout);
    Vayout->setSpacing(0);
    Vayout->setMargin(0);
    setLayout(Vayout);
    setStyleSheet(UiStyle::ActionStyle);
}

void LayerWidget::initToolBar()
{
    layerToolBar = new QWidget(this);
    layerToolBar->setAutoFillBackground(true);
    QPalette pale = layerToolBar->palette();
    pale.setColor(QPalette::Window, UiStyle::ToolBarColor);
    layerToolBar->setPalette(pale);
    layerToolBar->setMinimumWidth(10);
    layerToolBar->setMaximumHeight(20);
    layerButton = new PushButton(layerToolBar);
    layerButton->setIcon(QIcon(":/dfjy/images/layout.png"));
    layerMenu = new QMenu(layerToolBar);
    QHBoxLayout *HLayout = new QHBoxLayout(layerToolBar);

    loadAction = new QAction("Load LayerSet", layerMenu);
    saveAction = new QAction("Save LayerSet", layerMenu);
    reassignAction = new QAction("Reassign LayerSet", layerMenu);
    layerMenu->addAction(loadAction);
    layerMenu->addAction(saveAction);
    layerMenu->addAction(reassignAction);
    layerButton->setMenu(layerMenu);

    SaveButton = new PushButton(layerToolBar);
    SaveButton->setIcon(QIcon(":/dfjy/images/quick.png"));
    SaveMenu = new QMenu(layerToolBar);
    QuickloadAction1 = new QAction("[1] Quick Load", SaveMenu);
    QuickloadAction2 = new QAction("[2] Quick Load", SaveMenu);
    QuickloadAction3 = new QAction("[3] Quick Load", SaveMenu);
    QuicksaveAction1 = new QAction("[1] Quick Save", SaveMenu);
    QuicksaveAction2 = new QAction("[2] Quick Save", SaveMenu);
    QuicksaveAction3 = new QAction("[3] Quick Save", SaveMenu);
    SaveMenu->addAction(QuickloadAction1);
    SaveMenu->addAction(QuickloadAction2);
    SaveMenu->addAction(QuickloadAction3);
    SaveMenu->addSeparator();
    SaveMenu->addAction(QuicksaveAction1);
    SaveMenu->addAction(QuicksaveAction2);
    SaveMenu->addAction(QuicksaveAction3);
    SaveButton->setMenu(SaveMenu);

    layerCombox = new Commbox(layerToolBar);
    layerCombox->addItem("default");
    layerCombox->addItem("5-colors");
    layerCombox->addItem("transparent");
    linkButton = new PushButton(layerToolBar);
    linkButton->setIcon(QIcon(":/dfjy/images/link.png"));
    LayerControlButton = new PushButton(layerToolBar);
    LayerControlButton->setCheckable(true);
    LayerControlButton->setIcon(QIcon(":/dfjy/images/showColor.png"));
    HLayout->addWidget(layerButton);
    HLayout->addWidget(layerCombox);
    HLayout->addWidget(linkButton);
    HLayout->addWidget(LayerControlButton);
    HLayout->addSpacerItem(new QSpacerItem(54, 15, QSizePolicy::Fixed, QSizePolicy::Fixed));
    HLayout->addWidget(SaveButton);
    HLayout->setContentsMargins(0, 0, 0, 0);
    HLayout->setSpacing(0);

    connect(LayerControlButton, SIGNAL(clicked(bool)), this, SLOT(slot_showLayerControlWidget(bool)));
}

void LayerWidget::initTree()
{
    TreeHLayout = new QHBoxLayout();
    TreeHLayout->setContentsMargins(0, 0, 0, 0);
    layerTree = new QTreeView(this);
    TreeHLayout->addWidget(layerTree);
    layerTreeModel = new QStandardItemModel(layerTree);

    layerTree->setModel(layerTreeModel);
    layerTreeModel->setHorizontalHeaderLabels(QStringList() << "Name"
                                          << "Icon"<< "Type"<< "Desc");

    layerTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(layerTree, SIGNAL(customContextMenuRequested(const QPoint& )), this, SLOT(slot_layerContextMenu(const QPoint&)));
    connect (layerTreeModel ,SIGNAL(itemChanged(QStandardItem*)), this , SLOT(slot_treeItemChanged(QStandardItem*)));
}

void LayerWidget::slot_treeItemChanged(QStandardItem *item)
{
    if (item == NULL)
        return;
    if (item -> isCheckable ())
    {
        Qt::CheckState state = item->checkState ();
        if (state != Qt :: PartiallyChecked )
        {
            treeItem_checkAllChild (item, state == Qt :: Checked ? true : false );
        }
        else
        {
            treeItem_CheckChildChanged (item);
        }
    }
}

void LayerWidget::treeItem_checkAllChild(QStandardItem * item, bool check)
{
    if(item == NULL)
        return;

    int rowCount = item->rowCount();

    for(int i=0;i<rowCount;++i)
    {
        QStandardItem* childItems = item->child(i);
        treeItem_checkAllChild_recursion(childItems,check);
    }

    if(item->isCheckable())
        item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
}

void LayerWidget::treeItem_CheckChildChanged(QStandardItem *item)
{
    if(NULL == item)
        return;

    Qt::CheckState siblingState = checkSibling(item);
    QStandardItem * parentItem = item->parent();

    if(NULL == parentItem)
        return;
    if(Qt::PartiallyChecked == siblingState)
    {
        if(parentItem->isCheckable() && parentItem->isTristate())
            parentItem->setCheckState(Qt::PartiallyChecked);
    }
    else if(Qt::Checked == siblingState)
    {
        if(parentItem->isCheckable())
            parentItem->setCheckState(Qt::Checked);
    }
    else
    {
        if(parentItem->isCheckable())
            parentItem->setCheckState(Qt::Unchecked);
    }

    treeItem_CheckChildChanged(parentItem);
}

Qt::CheckState LayerWidget::checkSibling(QStandardItem *item)
{
    QStandardItem * parent = item->parent();
    if(NULL == parent)
        return item->checkState();

    int brotherCount = parent->rowCount();

    int checkedCount(0),unCheckedCount(0);

    Qt::CheckState state;
    for(int i=0;i<brotherCount;++i)
    {
        QStandardItem* siblingItem = parent->child(i);
        state = siblingItem->checkState();
        if(Qt::PartiallyChecked == state)
            return Qt::PartiallyChecked;
        else if(Qt::Unchecked == state)
            ++ unCheckedCount;
        else
            ++ checkedCount;

        if(checkedCount > 0 && unCheckedCount > 0)
            return Qt::PartiallyChecked;
    }

    if(unCheckedCount > 0)
        return Qt::Unchecked;
    return Qt::Checked;
}

void LayerWidget::treeItem_checkAllChild_recursion(QStandardItem * item,bool check)
{
    if(item == NULL)
        return;

    int rowCount = item->rowCount();

    for(int i=0;i<rowCount;++i)
    {
        QStandardItem* childItems = item->child(i);
        treeItem_checkAllChild_recursion(childItems,check);
    }

    if(item->isCheckable())
        item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
}

void LayerWidget::slot_layerUpdata(QString currentFile)
{
    QStandardItem *rootFileItem = new QStandardItem(currentFile);
    rootFileItem->setCheckable(true);
    layerTreeModel->appendRow(rootFileItem);

    QStandardItem* pStandardItem = NULL;
    QStandardItem* pStandardChildItem = NULL;

    for (int i = 0; i < 20; i ++ )
    {
        pStandardItem = new QStandardItem(QString::number(rand() % 10000));
        pStandardItem->setCheckable(true);
        rootFileItem->appendRow(pStandardItem);
        rootFileItem->setChild(pStandardItem->row(), pStandardItem);

        for (int j = 0; j < 4; j ++)
        {
            pStandardChildItem = new QStandardItem(QString::number(rand() % 10000));
            pStandardChildItem->setCheckable(true);
            pStandardItem->appendRow(pStandardChildItem);
            for (int k = 0; k < 4; k ++)
            {
                pStandardItem->setChild(pStandardChildItem->row(), 3 ,new QStandardItem("DFEFSDEWWESDF"));
                pStandardItem->setChild(pStandardChildItem->row(), 2 ,new QStandardItem("ASDASFASSFDQW"));
                pStandardItem->setChild(pStandardChildItem->row(), 1 ,new QStandardItem(QIcon(":/new/prefix1/images/left.png"), ""));
            }
        }
    }
}

void LayerWidget::slot_layerContextMenu(const QPoint &pos)
{
    QStringList MenuTextList;
    MenuTextList << "color"  << "Text Color" << "Line Color"
                 << "Fill Style" << "Line Style" << "Line Width" << "Custom Style"
                 << "On" << "All on" << "All of" << "Invert All On/Off"
                 << "select Only" << "Status" << "Expand Layer Tree" << "Collapse Layer Tree"
                 << "Filtering" << "Group" << "Ungroup" << "Order" << "Clipboaed"
                 << "Edit Name" << "Edit Comment";
    QVector <QAction *> actionList;

    QMenu Layermenu;

    for (int i = 0; i < MenuTextList.count(); i ++)
    {
        QAction *act = new QAction(MenuTextList.at(i), &Layermenu);
        if (i == 7 || i == 13) Layermenu.addSeparator();
        if (i < 7 || i == 12 || i == 15 || i == 18 || i == 19)
        {
            if(i < 3)
            {
                MenuColor *menu = new MenuColor(MenuTextList.at(i));
                act->setMenu(menu);
            }
            else if (i == 3)
            {
                MenuStyle *menu = new MenuStyle(MenuTextList.at(i));
                act->setMenu(menu);
            }
            else if (i == 4)
            {
                QMenu *menu = new QMenu(this);
                //                QPen *pen1 = new QPen(Qt::NoPen);
                //                QPen *pen2 = new QPen(Qt::SolidLine);
                //                QPen *pen3 = new QPen(Qt::DashLine);
                //                QPen *pen4 = new QPen(Qt::DotLine);
                //                QPen *pen5 = new QPen(Qt::DashDotLine);
                //                QPen *pen6 = new QPen(Qt::DashDotDotLine);
                //                QPen *pen7 = new QPen(Qt::CustomDashLine);
                none = new QAction(QIcon(":/dfjy/images/line_style_1.png"), "none", this);
                dashdotdot = new QAction(QIcon(":/dfjy/images/line_style_2.png"), "dashdotdot", this);
                solid = new QAction(QIcon(":/dfjy/images/line_style_3.png"), "solid", this);
                dash = new QAction(QIcon(":/dfjy/images/line_style_4.png"), "dash", this);
                dot = new QAction(QIcon(":/dfjy/images/line_style_5.png"), "dot", this);
                none->setCheckable(true);
                none->setChecked(true);
                menu->addAction(none);
                dashdotdot->setCheckable(true);
                menu->addAction(dashdotdot);
                solid->setCheckable(true);
                menu->addAction(solid);
                dash->setCheckable(true);
                menu->addAction(dash);
                dot->setCheckable(true);
                menu->addAction(dot);
                act->setMenu(menu);
            }
            else if(i == 5)
            {
                QMenu *menu = new QMenu(this);
                linewihthAction1 = new QAction(QIcon(":/dfjy/images/line_width1.png"), "1", this);
                linewihthAction1->setCheckable(true);
                linewihthAction1->setChecked(true);
                menu->addAction(linewihthAction1);
                linewihthAction2 = new QAction(QIcon(":/dfjy/images/line_width2.png"), "2", this);
                linewihthAction2->setCheckable(true);
                menu->addAction(linewihthAction2);
                linewihthAction3 = new QAction(QIcon(":/dfjy/images/line_width3.png"), "3", this);
                linewihthAction3->setCheckable(true);
                menu->addAction(linewihthAction3);
                linewihthAction4 = new QAction(QIcon(":/dfjy/images/line_width4.png"), "4", this);
                linewihthAction4->setCheckable(true);
                menu->addAction(linewihthAction4);
                act->setMenu(menu);
            }
        }
        Layermenu.addAction(act);
        actionList.append(act);
    }

    QPoint tempPos = pos;
    tempPos.setY(tempPos.y() + 22);
    Layermenu.exec(layerTree->mapToGlobal(tempPos));
}

void LayerWidget::slot_showLayerControlWidget(bool ischeck)
{
    if(ischeck)
    {
        LayerControlWidget = new QWidget(this);
        LayerControlWidget->setMaximumWidth(215);

        QVBoxLayout *Vlayout = new QVBoxLayout(LayerControlWidget);
        QWidget *ButtonBar = new QWidget(LayerControlWidget);
        ButtonOn= new PushButton(ButtonBar);
        ButtonExpose= new PushButton(ButtonBar);
        ButtonSelectable= new PushButton(ButtonBar);
        ButtonText= new PushButton(ButtonBar);
        ButtonNage= new PushButton(ButtonBar);
        ButtonCut= new PushButton(ButtonBar);
        ButtonOn->setCheckable(true);
        ButtonExpose->setCheckable(true);
        ButtonSelectable->setCheckable(true);
        ButtonText->setCheckable(true);
        ButtonNage->setCheckable(true);
        ButtonCut->setCheckable(true);
        ButtonOn->setIcon(QIcon(":/dfjy/images/on.png"));
        ButtonExpose->setIcon(QIcon(":/dfjy/images/layer.png"));
        ButtonSelectable->setIcon(QIcon(":/dfjy/images/select.png"));
        ButtonText->setIcon(QIcon(":/dfjy/images/text.png"));
        ButtonNage->setIcon(QIcon(":/dfjy/images/nega.png"));
        ButtonCut->setIcon(QIcon(":/dfjy/images/cut.png"));
        QHBoxLayout *Hlayout = new QHBoxLayout(ButtonBar);
        Hlayout->setContentsMargins(0, 0, 0, 0);
        Hlayout->setSpacing(0);
        Hlayout->addWidget(ButtonOn);
        Hlayout->addWidget(ButtonExpose);
        Hlayout->addWidget(ButtonSelectable);
        Hlayout->addWidget(ButtonText);
        Hlayout->addWidget(ButtonNage);
        Hlayout->addWidget(ButtonCut);

        QTabWidget* ColorTable = new QTabWidget(LayerControlWidget);
        ColorWidget *colorwidget = new ColorWidget(ColorTable);
        ColorWidget *Linecolorwidget = new ColorWidget(ColorTable);
        ColorWidget *Textcolorwidget = new ColorWidget(ColorTable);
        ColorTable->addTab(colorwidget, "Color");
        ColorTable->addTab(Linecolorwidget, "LineColor");
        ColorTable->addTab(Textcolorwidget, "TextColor");

        QTabWidget* styleTable = new QTabWidget(LayerControlWidget);
        StyleWidget *styleWidget = new StyleWidget(styleTable);
        styleTable->addTab(styleWidget, "FillStyle");

        QHBoxLayout *lineStyleLayout = new QHBoxLayout();
        QLabel *LineStyleLabel = new QLabel("Line Style:",LayerControlWidget);
        LineStyleCommbox = new Commbox(this);
        LineStyleCommbox->addItem(QIcon(":/dfjy/images/line_style_1.png"), "none");
        LineStyleCommbox->addItem(QIcon(":/dfjy/images/line_style_2.png"), "dashdotdot");
        LineStyleCommbox->addItem(QIcon(":/dfjy/images/line_style_3.png"), "solid");
        LineStyleCommbox->addItem(QIcon(":/dfjy/images/line_style_4.png"), "dash");
        LineStyleCommbox->addItem(QIcon(":/dfjy/images/line_style_5.png"), "dot");
        lineStyleLayout->setContentsMargins(0, 0, 0, 0);
        lineStyleLayout->setSpacing(0);
        lineStyleLayout->addWidget(LineStyleLabel);
        lineStyleLayout->addWidget(LineStyleCommbox);

        QHBoxLayout *lineWidthLayout = new QHBoxLayout();
        QLabel *LineWidthLabel = new QLabel("Line Width:",LayerControlWidget);
        LineWidthCommbox = new Commbox(this);
        LineWidthCommbox->addItem(QIcon(":/dfjy/images/line_width1.png"), "1");
        LineWidthCommbox->addItem(QIcon(":/dfjy/images/line_width2.png"), "2");
        LineWidthCommbox->addItem(QIcon(":/dfjy/images/line_width3.png"), "3");
        LineWidthCommbox->addItem(QIcon(":/dfjy/images/line_width4.png"), "4");
        lineWidthLayout->setContentsMargins(0, 0, 0, 0);
        lineWidthLayout->addWidget(LineWidthLabel);
        lineWidthLayout->addWidget(LineWidthCommbox);

        QHBoxLayout *TransparencyLayout = new QHBoxLayout();
        QLabel *TransparencyLabel = new QLabel("Transparency:",LayerControlWidget);
        TransparencyCommbox = new Commbox(this);
        TransparencyCommbox->addItem("0");
        TransparencyCommbox->addItem("20");
        TransparencyCommbox->addItem("50");
        TransparencyCommbox->addItem("70");
        TransparencyCommbox->addItem("90");
        TransparencyLayout->setContentsMargins(0, 0, 0, 0);
        TransparencyLayout->addWidget(TransparencyLabel);
        TransparencyLayout->addWidget(TransparencyCommbox);

        QSplitter*splitter = new QSplitter(LayerControlWidget);
        splitter->setOrientation(Qt::Vertical);
        splitter->addWidget(ButtonBar);
        splitter->addWidget(ColorTable);
        splitter->addWidget(styleTable);

        Vlayout->setContentsMargins(0, 0, 0, 0);
        Vlayout->setSpacing(5);
        Vlayout->addWidget(splitter);
        Vlayout->addLayout(lineStyleLayout);
        Vlayout->addLayout(lineWidthLayout);
        Vlayout->addLayout(TransparencyLayout);
        TreeHLayout->addWidget(LayerControlWidget);
    }
    else
    {
        if (LayerControlWidget != NULL)
        {
            TreeHLayout->removeWidget(LayerControlWidget);
            delete LayerControlWidget;
        }
    }
}

