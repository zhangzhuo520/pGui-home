#include "ui_layer_widget.h"
#include "render_frame.h"

namespace ui {
LayerWidget::LayerWidget(QWidget *parent) :
    QWidget(parent),
    m_active_model_index(0),
    m_active_model_rootIndex(-1)
{
    initToolBar();
    initTree();
    init_layercontral_widget();
    QVBoxLayout *Vayout = new QVBoxLayout(this);

    Vayout->addWidget(layerToolBar);
    Vayout->addLayout(TreeHLayout);
    Vayout->setSpacing(0);
    Vayout->setMargin(0);
    setLayout(Vayout);
}

void LayerWidget::initToolBar()
{
    layerToolBar = new QWidget(this);
    layerToolBar->setAutoFillBackground(true);
    QPalette pale = layerToolBar->palette();
    pale.setColor(QPalette::Window, UiStyle::ToolBarColor);
    layerToolBar->setPalette(pale);
    QHBoxLayout *HLayout = new QHBoxLayout(layerToolBar);
    LayerControlButton = new PushButton(layerToolBar);
    LayerControlButton->setCheckable(true);
    LayerControlButton->setIcon(QIcon(":/dfjy/images/showColor.png"));
    HLayout->addWidget(LayerControlButton);
    HLayout->addSpacerItem(new QSpacerItem(180, 15, QSizePolicy::Fixed, QSizePolicy::Fixed));
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
    layerTreeModel = new TreeModel(layerTree);
    layerTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layerTreeModel->setHorizontalHeaderLabels(QStringList() << "Name"
                                          << "Icon"<< "Desc");

    layerTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(layerTree, SIGNAL(customContextMenuRequested(const QPoint& )), this, SLOT(slot_layerContextMenu(const QPoint&)));
    connect(layerTreeModel ,SIGNAL(itemChanged(QStandardItem*)), this , SLOT(slot_treeItemChanged(QStandardItem*)));
    connect(layerTree ,SIGNAL(doubleClicked(QModelIndex)), this , SLOT(slot_treeDoubleClick(QModelIndex)));
    connect(layerTree, SIGNAL(pressed(QModelIndex)), this, SLOT(slot_activedModelIndex(QModelIndex)));
    layerTree->setModel(layerTreeModel);
}

void LayerWidget::init_layercontral_widget()
{
    LayerControlWidget = new QWidget(this);
    LayerControlWidget->setMaximumWidth(215);

    QVBoxLayout *Vlayout = new QVBoxLayout(LayerControlWidget);

    QTabWidget* ColorTable = new QTabWidget(LayerControlWidget);
    ColorWidget *colorwidget = new ColorWidget(ColorTable);
    ColorWidget *Linecolorwidget = new ColorWidget(ColorTable);
    ColorWidget *Textcolorwidget = new ColorWidget(ColorTable);
    ColorTable->addTab(colorwidget, "Fill");
    ColorTable->addTab(Linecolorwidget, "Line");
    ColorTable->addTab(Textcolorwidget, "Text");

    QTabWidget* styleTable = new QTabWidget(LayerControlWidget);
    StyleWidget *styleWidget = new StyleWidget(styleTable);
    styleTable->addTab(styleWidget, "FillStyle");

    QHBoxLayout *lineStyleLayout = new QHBoxLayout();
    QLabel *LineStyleLabel = new QLabel("Line Style:",LayerControlWidget);
    LineStyleCommbox = new Commbox(this);
    LineStyleCommbox->addItem(QPixmap::fromImage(set_line_image(0)), "none");
    LineStyleCommbox->addItem(QPixmap::fromImage(set_line_image(1)), "long dashed");
    LineStyleCommbox->addItem(QPixmap::fromImage(set_line_image(2)), "dashed");
    LineStyleCommbox->addItem(QPixmap::fromImage(set_line_image(3)), "short dash-dotted");
    LineStyleCommbox->addItem(QPixmap::fromImage(set_line_image(4)), "dotted");
    LineStyleCommbox->addItem(QPixmap::fromImage(set_line_image(5)), "dash-dotted");
    LineStyleCommbox->addItem(QPixmap::fromImage(set_line_image(6)), "short dashed");
    LineStyleCommbox->addItem(QPixmap::fromImage(set_line_image(7)), "dash-double-dotted");
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

    connect(LineWidthCommbox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setLineWidth(int)));
    connect(LineStyleCommbox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setLineStyle(int)));
    LayerControlWidget->hide();
}

void LayerWidget::slot_activedModelIndex(QModelIndex index)
{
    m_active_model_index =  index.row();
    m_active_model_rootIndex = index.parent().row();
}

void LayerWidget::slot_treeItemChanged(QStandardItem *item)
{
    if (item == NULL)
        return;
    if (item -> isCheckable ())
    {
        Qt::CheckState state = item->checkState ();
        if (item->hasChildren())
        {
            treeItem_checkAllChild (item, state == Qt :: Checked ? true : false );
        }
        else
        {
            item_checked(item);
        }
    }
}

void LayerWidget::slot_treeDoubleClick(QModelIndex item)
{
//    TIME_DEBUG
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
        if(childItems->isCheckable())
        {
            childItems->setCheckState(check ? Qt::Checked : Qt::Unchecked);
            item_checked(childItems);
        }
    }
}

void LayerWidget::slot_layerContextMenu(const QPoint &pos)
{
    QStringList MenuTextList;
    MenuTextList << "color"      << "Text Color" << "Line Color"
                 << "Fill Style" << "Line Style" << "Line Width";
    QVector <QAction *> actionList;

    QMenu *Layermenu = new QMenu();

    for (int i = 0; i < MenuTextList.count(); i ++)
    {
        QAction *act = new QAction(MenuTextList.at(i), Layermenu);
        if (i == 7 || i == 13) Layermenu->addSeparator();
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
                none->setData(QVariant(0));
                dashdotdot->setData(QVariant(1));
                solid->setData(QVariant(2));
                dash->setData(QVariant(3));
                dot->setData(QVariant(4));

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
//                linewihthAction1->setCheckable(true);
//                linewihthAction1->setChecked(true);
                menu->addAction(linewihthAction1);
                linewihthAction2 = new QAction(QIcon(":/dfjy/images/line_width2.png"), "2", this);
//                linewihthAction2->setCheckable(true);
                menu->addAction(linewihthAction2);
                linewihthAction3 = new QAction(QIcon(":/dfjy/images/line_width3.png"), "3", this);
//                linewihthAction3->setCheckable(true);
                menu->addAction(linewihthAction3);
                linewihthAction4 = new QAction(QIcon(":/dfjy/images/line_width4.png"), "4", this);
//                linewihthAction4->setCheckable(true);
                menu->addAction(linewihthAction4);
                act->setMenu(menu);
                for (int i = 0; i < menu->actions().count(); i ++)
                {
                    connect(menu->actions().at(i), SIGNAL(triggered()), this, SLOT(slot_LineStyle_action()));
                }

                for (int i = 0; i < menu->actions().count(); i ++)
                {
                    connect(menu->actions().at(i), SIGNAL(triggered()), this, SLOT(slot_LineWidth_action()));
                }

            }
        }
        Layermenu->addAction(act);
        actionList.append(act);
    }

    QPoint tempPos = pos;
    tempPos.setY(tempPos.y() + 22);
    Layermenu->exec(layerTree->mapToGlobal(tempPos));
}

void LayerWidget::slot_showLayerControlWidget(bool ischeck)
{
    if(ischeck)
    {
        LayerControlWidget->show();
    }
    else
    {
        LayerControlWidget->hide();
    }
}


void LayerWidget::slot_setBackgroundColor(QColor color)
{
    if (m_layer_style_vector.isEmpty())
    {
        return;
    }

    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }

    int index = m_view->tree_to_list_index(m_active_model_rootIndex, m_active_model_index);
    layerstyle m_layerstyle = m_layer_style_vector.at(index);
    m_layerstyle.fill_color = color_to_uint(color);
    m_layer_style_vector[index] = m_layerstyle;
    QImage image = set_fill_image(m_layerstyle);
    setModelIdexImage(image);
    setLayerData(m_layerstyle);
}

void LayerWidget::slot_setLineColor(QColor color)
{
    if (m_layer_style_vector.isEmpty())
    {
        return;
    }

    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }

    int index = m_view->tree_to_list_index(m_active_model_rootIndex, m_active_model_index);
    layerstyle m_layerstyle = m_layer_style_vector.at(index);
    m_layerstyle.frame_color = color_to_uint(color);
    m_layer_style_vector[index] = m_layerstyle;
    setModelIdexImage(set_fill_image(m_layerstyle));
    setLayerData(m_layerstyle);
}

void LayerWidget::slot_setLineStyle(int line_style)
{
    if (m_layer_style_vector.isEmpty())
    {
        return;
    }

    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }

    int index = m_view->tree_to_list_index(m_active_model_rootIndex, m_active_model_index);
    layerstyle m_layerstyle = m_layer_style_vector.at(index);
    m_layerstyle.line_style = line_style;
    m_layer_style_vector[index] = m_layerstyle;
    setLayerData(m_layerstyle);
}

void LayerWidget::slot_LineWidth_action()
{
    QAction * action = static_cast <QAction *> (sender());

    if (action->text().toInt() == LineWidthCommbox->currentText().toInt())
    {
        slot_setLineWidth(action->text().toInt());
    }
    else
    {
        LineWidthCommbox->setCurrentIndex(action->text().toInt() - 1);
    }
}

void LayerWidget::slot_LineStyle_action()
{
    QAction* action = static_cast<QAction*> (sender());
    slot_setLineStyle(action->data().toInt() - 1);
}

void LayerWidget::slot_setLayerStyle(int patternIdex)
{
    if (m_layer_style_vector.isEmpty())
    {
        return;
    }

    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }

    int index = m_view->tree_to_list_index(m_active_model_rootIndex, m_active_model_index);
    layerstyle m_layerstyle = m_layer_style_vector.at(index);
    m_layerstyle.pattern_Id = patternIdex;
    m_layer_style_vector[index] = m_layerstyle;
    QImage image = set_fill_image(m_layerstyle);
    setModelIdexImage(image);
    setLayerData(m_layerstyle);
}

void LayerWidget::slot_setTextColor(QColor color)
{
    Q_UNUSED(color);
}

void LayerWidget::item_checked(QStandardItem* index)
{
    m_active_model_index = index->row();
    if(index->parent())
    {
        m_active_model_rootIndex = index->parent()->row();
    }
    else
    {
        m_active_model_rootIndex = 0;
    }
    if(index->isCheckable())
    {
        if (index->checkState() == Qt::Unchecked)
        {
            setItemChecked(false);
        }
        else if (index->checkState() == Qt::Checked)
        {
            setItemChecked(true);
        }
    }
}

const QStringList &LayerWidget::get_all_layername()
{
    return m_all_layername_list;
}

void LayerWidget::slot_setLineWidth(int line_width)
{
    if (m_layer_style_vector.isEmpty())
    {
        return;
    }

    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }

    int index = m_view->tree_to_list_index(m_active_model_rootIndex, m_active_model_index);
    layerstyle m_layerstyle = m_layer_style_vector.at(index);
    m_layerstyle.line_width = line_width + 1;
    m_layer_style_vector[index] = m_layerstyle;
    setLayerData(m_layerstyle);
}

void LayerWidget::setItemChecked(bool check)
{
    if (m_layer_style_vector.isEmpty())
    {
        return;
    }

    if(m_active_model_index == -1)
    {
        showWarning(this, "Warning", "Not select Item !");
        return;
    }

    int index = m_view->tree_to_list_index(m_active_model_rootIndex, m_active_model_index);
    layerstyle m_layerstyle = m_layer_style_vector.at(index);
    m_layerstyle.isVisible = check;
    m_layer_style_vector[index] = m_layerstyle;
    setLayerData(m_layerstyle);
}

void LayerWidget::getLayerData(render::RenderFrame* view)
{
    for(int i = 0; i < layerTreeModel->rowCount(); i++)
    {
        layerTreeModel->removeRow(0);
    }
    rootItem_vector.clear();
    m_all_layername_list.clear();

    if(NULL == view || view->layout_views_size() == 0)
    {
        return;
    }
    m_all_layername_list.clear();
    m_layer_style_vector.clear();
    m_view = view;

    for(int i = 0; i < view->layout_views_size(); i++)
    {
        QString currentFile = view->get_layout_view(i)->file_name().c_str();
        TreeItem* fileItem = new TreeItem(currentFile);
        rootItem_vector.append(fileItem);
        bool all_checked = true;

        for(uint i = 0; i < view->layers_size(); i++)
        {
            if(!view->get_properties(i)->visible())
            {
                all_checked = false;
                break;
            }
        }

        fileItem->setCheckState(all_checked ? Qt::Checked: Qt::Unchecked);
        fileItem->setCheckable(true);
        fileItem->setEditable(false);

        layerTreeModel->setItem(i, fileItem);

        TreeItem* pStandardItem = NULL;

        for (uint j = 0; j < view->layers_size(); j++)
        {
            if(view->get_properties(j)->view_index() == i)
            {
                QString layerName = QString::fromStdString(view->get_properties(j)->metadata().get_layer_name());
                QString dataType = QString::number(view->get_properties(j)->metadata().get_data_type());
                QString layerNum = QString::number(view->get_properties(j)->metadata().get_layer_num());

                pStandardItem = new TreeItem(layerNum + "/" + dataType);
                pStandardItem->setCheckable(true);
                pStandardItem->setEditable(false);

                layerStyle m_layer_style;
                m_layer_style.frame_color = view->get_properties(j)->frame_color();
                m_layer_style.fill_color = view->get_properties(j)->fill_color();
                m_layer_style.pattern_Id = view->get_properties(j)->pattern();
                m_layer_style.line_width = view->get_properties(j)->line_width();
                m_layer_style.line_style = view->get_properties(j)->line_style();
                m_layer_style.isVisible = view->get_properties(j)->visible();

                m_layer_style_vector.append(m_layer_style);
                QImage image = set_fill_image(m_layer_style);

                if(m_layer_style.isVisible)
                {
                    pStandardItem->setCheckState(Qt::Checked);
                }
                else
                {
                    pStandardItem->setCheckState(Qt::Unchecked);
                }

                fileItem->appendRow(pStandardItem);
                TreeItem *childItem2 = new TreeItem(layerName);
                childItem2->setCheckable(false);
                childItem2->setEditable(false);
                fileItem->setChild(pStandardItem->row(), 2, childItem2);
                TreeItem *childItem1 = new TreeItem();
                childItem1->setCheckable(false);
                childItem1->setEditable(false);
                childItem1->setData(image, Qt::DecorationRole);
                fileItem->setChild(pStandardItem->row(), 1, childItem1);
                m_all_layername_list.append(currentFile + " " + layerNum + "/" + dataType);
            }

        }

        emit signal_update_layername_list(m_all_layername_list);
    }

    layerTree->setColumnWidth(1, 40);
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

QImage LayerWidget::set_fill_image(layerStyle m_layer_style)
{
    QImage image = pattern.get_bitmap(m_layer_style.pattern_Id, 36, 18).toImage().convertToFormat(QImage::Format_RGB32);
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
            if (!(temp_color.red() == 255
                    && temp_color.blue() == 255
                    && temp_color.green() == 255))
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

QImage LayerWidget::set_line_image(int line_style)
{
    return  m_line_style.get_bitmap(line_style, 48, 36).toImage().convertToFormat(QImage::Format_RGB32);
}

render::RenderFrame *LayerWidget::get_renderFrame()
{
    return m_view;
}

void LayerWidget::setLayerData(layerStyle layer_style)
{
    if (m_active_model_rootIndex < 0 || rootItem_vector.isEmpty())
    {
        return;
    }
    int index = m_view->tree_to_list_index(m_active_model_rootIndex, m_active_model_index);

    render::LayerProperties layerProperty = *(m_view->get_properties(index));
    layerProperty.set_fill_color(layer_style.fill_color);
    layerProperty.set_frame_color(layer_style.frame_color);
    layerProperty.set_pattern(layer_style.pattern_Id);
    layerProperty.set_visible(layer_style.isVisible);
    layerProperty.set_line_width(layer_style.line_width);
    layerProperty.set_line_style(layer_style.line_style);

    m_view->set_current_layer(index);
    m_view->set_properties(index, layerProperty);

}

void LayerWidget::setModelIdexImage(QImage image)
{
    TreeItem *childItem = new TreeItem();
    childItem->setData(image, Qt::DecorationRole);
    if (m_active_model_rootIndex < 0 || rootItem_vector.isEmpty())
    {
        return;
    }

    TreeItem *rootItem = rootItem_vector.at(m_active_model_rootIndex);
    rootItem->child(m_active_model_index, 1)->removeRow(0);
    rootItem->setChild(m_active_model_index, 1, childItem);
}
}
