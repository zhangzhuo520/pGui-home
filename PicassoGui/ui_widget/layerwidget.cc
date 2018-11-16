#include "layerwidget.h"
namespace UI {
LayerWidget::LayerWidget(QWidget *parent) :
    QWidget(parent),
    m_active_model_index(-1)
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
    layerTreeModel = new LayerTreeModel(layerTree);

    layerTree->setModel(layerTreeModel);
    layerTreeModel->setHorizontalHeaderLabels(QStringList() << "Name"
                                          << "Icon"<< "Type"<< "Desc");

    layerTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(layerTree, SIGNAL(customContextMenuRequested(const QPoint& )), this, SLOT(slot_layerContextMenu(const QPoint&)));
    connect (layerTreeModel ,SIGNAL(itemChanged(QStandardItem*)), this , SLOT(slot_treeItemChanged(QStandardItem*)));
    connect (layerTree ,SIGNAL(doubleClicked(QModelIndex)), this , SLOT(slot_treeDoubleClick(QModelIndex)));
    connect (layerTree, SIGNAL(pressed(QModelIndex)), this, SLOT(slot_activedModerIndex(QModelIndex)));
}

void LayerWidget::slot_activedModerIndex(QModelIndex index)
{
    m_active_model_index =  index.row();
    activeModelIndex = &index;
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

void LayerWidget::slot_treeDoubleClick(QModelIndex item)
{
    //QlayerPropertyVctor.at(i).set_pattern();
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

void LayerWidget::slot_layerContextMenu(const QPoint &pos)
{
    QStringList MenuTextList;
    MenuTextList << "color"      << "Text Color" << "Line Color"
                 << "Fill Style" << "Line Style" << "Line Width"        << "Custom Style"
                 << "On"         << "All on"     << "All of"            << "Invert All On/Off"
                 << "select Only"<< "Status"     << "Expand Layer Tree" << "Collapse Layer Tree"
                 << "Filtering"  << "Group"      << "Ungroup"           << "Order" << "Clipboaed"
                 << "Edit Name"  << "Edit Comment";
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
                if(i == 0)
                {
                    connect(menu, SIGNAL(signal_selectColor(QColor)), this, SLOT(slot_setBackgroundColor(QColor)));
                }
                if(i == 1)
                {
                    connect(menu, SIGNAL(signal_selectColor(QColor)), this, SLOT(slot_setTextColor(QColor)));
                }
                if(i == 2)
                {
                    connect(menu, SIGNAL(signal_selectColor(QColor)), this, SLOT(slot_setLineColor(QColor)));
                }

            }
            else if (i == 3)
            {
                MenuStyle *menu = new MenuStyle(MenuTextList.at(i));
                act->setMenu(menu);
                connect(menu, SIGNAL(signal_selectItemStyle(int)), this, SLOT(slot_setLayerStyle(int)));
            }
            else if (i == 4)
            {
                QMenu *menu = new QMenu(this);
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
        connect(colorwidget, SIGNAL(signal_selectColor(QColor)), this, SLOT(slot_setBackgroundColor(QColor)));
        connect(Linecolorwidget, SIGNAL(signal_selectColor(QColor)), this, SLOT(slot_setLineColor(QColor)));
        connect(styleWidget, SIGNAL(signal_selectItemStyle(int)), this, SLOT(slot_setLayerStyle(int)));
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

void LayerWidget::slot_getLayerData(std::vector<render::LayerProperties> layerProprtList, QString currentFile)
{
    getLayerData(layerProprtList, currentFile);
}

void LayerWidget::slot_setBackgroundColor(QColor color)
{
    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }
    layerstyle m_layerstyle = m_layer_style_vector.at(m_active_model_index);
    m_layerstyle.fill_color = color_to_uint(color);
    qDebug() <<  " m_layerstyle: " << m_layerstyle.fill_color <<endl;
    m_layer_style_vector[m_active_model_index] = m_layerstyle;
    qDebug() << m_active_model_index << m_layer_style_vector[m_active_model_index].fill_color;
    QImage image = setImage(m_layerstyle);
    setModelIdexImage(image);
    setLayerData(m_layerstyle);
}

void LayerWidget::slot_setLineColor(QColor color)
{
    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }
    layerstyle m_layerstyle = m_layer_style_vector.at(m_active_model_index);
    m_layerstyle.frame_color = color_to_uint(color);
    m_layer_style_vector[m_active_model_index] = m_layerstyle;
    setModelIdexImage(setImage(m_layerstyle));
    setLayerData(m_layerstyle);
}

void LayerWidget::slot_setTextColor(QColor color)
{
//    if(m_active_model_index == -1)
//    {
//        showWarning(this, "Warning", "Not select Item !");
//        return;
//    }
//    layerstyle m_layerstyle = m_layer_style_vector.at(m_active_model_index);
//    m_layerstyle. = color_to_uint(color);
//    QImage image = setImage(m_layerstyle);
//    setModelIdexImage(image);
    //    setLayerData(m_layerstyle);
}

void LayerWidget::slot_setLayerStyle(int patternIdex)
{
    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }
    m_layer_style_vector[m_active_model_index].pattern_Id = patternIdex;
    QImage image = setImage(m_layer_style_vector[m_active_model_index]);
    setModelIdexImage(image);
    setLayerData(m_layer_style_vector[m_active_model_index]);
}

void LayerWidget::getLayerData(std::vector<render::LayerProperties> layerProprtList, QString currentFile)
{
    rootFileItem = new QStandardItem(currentFile);
    rootFileItem->setCheckable(true);
    layerTreeModel->setItem(0, rootFileItem);

    rootFileItem->setEditable(false);
    m_layer_property_vector.clear();
    QStandardItem* pStandardItem = NULL;

    for (uint i = 0; i < layerProprtList.size(); i ++ )
    {
        m_layer_property_vector.append(layerProprtList.at(i));
        qDebug() << "layer index :" << layerProprtList.at(i).layer_index();
        QString layerName = QString::fromStdString(layerProprtList.at(i).metadata().get_layer_name());
        QString dataType = QString::number(layerProprtList.at(i).metadata().get_data_type());
        QString layerData = QString::number(layerProprtList.at(i).metadata().get_layer_num());
        layerStyle m_layer_style;
        m_layer_style.frame_color = layerProprtList.at(i).frame_color();
        m_layer_style.fill_color = layerProprtList.at(i).fill_color();
        m_layer_style.pattern_Id = layerProprtList.at(i).pattern();
        m_layer_style.line_width = layerProprtList.at(i).width();
        m_layer_style.line_style = layerProprtList.at(i).line_style();
        m_layer_style_vector.append(m_layer_style);
        QImage image = setImage(m_layer_style);

        pStandardItem = new QStandardItem(layerData);
        pStandardItem->setEditable(false);
        pStandardItem->setCheckable(true);
        rootFileItem->appendRow(pStandardItem);

        QStandardItem *childItem3 = new QStandardItem(layerName);
        childItem3->setEditable(false);
        rootFileItem->setChild(pStandardItem->row(), 3, childItem3);
        QStandardItem *childItem2 = new QStandardItem(dataType);
        childItem2->setEditable(false);
        rootFileItem->setChild(pStandardItem->row(), 2, childItem2);
        QStandardItem *childItem1 = new QStandardItem(QIcon(QPixmap::fromImage(image)), "");
        childItem1->setEditable(false);
        rootFileItem->setChild(pStandardItem->row(), 1, childItem1);
    }
}

QColor LayerWidget::uint_to_color(uint color)
{
    int b = color << 24 >> 24;
    int g = color << 16 >> 24;
    int r = color << 8 >> 24;
    return QColor(r, g, b);
}

u_int LayerWidget::color_to_uint(QColor color)
{
    return qRgb(color.red(), color.green(), color.blue());
}

QImage LayerWidget::setImage(layerStyle m_layer_style)
{
    QImage image = pattern.get_bitmap(m_layer_style.pattern_Id, 32, 24).toImage().convertToFormat(QImage::Format_RGB32);
    QColor fill_color = uint_to_color(m_layer_style.fill_color);
    QColor frame_color = uint_to_color(m_layer_style.frame_color);
    for (int j = 0; j < image.height() - 1; j ++)
    {
        for (int i = 0; i < image.width() - 1; i ++)
        {
            QColor temp_color = uint_to_color(image.pixel(i, j));
#if 0
            if (!(temp_color.red() == 255
                    && temp_color.blue() == 255
                    && temp_color.green() == 255))
            {
                image.setPixel(QPoint(i, j), qRgb(fill_color.red(), fill_color.green(), fill_color.blue()));
            }
#else
            if (!(temp_color.red() == 0
                    && temp_color.blue() == 0
                    && temp_color.green() == 0))
            {
                image.setPixel(QPoint(i, j), qRgb(fill_color.red(), fill_color.green(), fill_color.blue()));
            }
#endif
        }
    }
    QPainter painter(&image);
    QPen pen;
    pen.setColor(frame_color);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawRect(0, 0, image.width() - 2, image.height() - 2);
    return image;
}

void LayerWidget::setLayerData(layerStyle layer_style)
{
    m_LayerProperty = m_layer_property_vector.at(m_active_model_index);
    m_LayerProperty.set_fill_color(layer_style.fill_color);
    m_LayerProperty.set_frame_color(layer_style.frame_color);
    m_LayerProperty.set_line_style(layer_style.line_style);
    m_LayerProperty.set_pattern(layer_style.pattern_Id);
    emit signal_setLayerData(m_LayerProperty);
}

void LayerWidget::setModelIdexImage(QImage image)
{
    pixmap = QPixmap::fromImage(image);
    QStandardItem *childItem = new QStandardItem(QIcon(QPixmap::fromImage(image)), "");
    rootFileItem->child(m_active_model_index, 1)->removeRow(0);
    rootFileItem->setChild(m_active_model_index, 1, childItem);
}
}
