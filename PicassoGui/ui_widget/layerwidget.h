#ifndef LayerWidget_H
#define LayerWidget_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTreeView>
#include <QLayout>
#include <QStringList>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QDebug>
#include <QIcon>
#include <QTabWidget>
#include <QSplitter>
#include <QBitmap>
#include <QPixmap>
#include <QImage>
#include <QPaintEvent>

#include "menuwidget.h"
#include "../renderer/render_layer_metadata.h"
#include "../renderer/render_layer_properties.h"
#include "../renderer/render_pattern.h"
#include "./deftools/cmessagebox.h"
#include "./deftools/model.h"
#include "./deftools/delegate.h"

namespace UI{

typedef struct layerstyle
{
    uint frame_color;
    uint fill_color;
    int pattern_Id;
    int line_width;
    int line_style;
    bool isVisible;
}layerStyle;

class LayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LayerWidget(QWidget *parent = 0);

    void initToolBar();

    void initTree();

    void treeItem_checkAllChild_recursion(QStandardItem * item,bool check);

    void treeItem_checkAllChild(QStandardItem * item, bool check);

    void treeItem_CheckChildChanged(QStandardItem * item);

    Qt::CheckState checkSibling(QStandardItem *item);

    QColor uint_to_color(uint color);

    uint color_to_uint(QColor);

    QImage setImage(layerStyle);

    void getLayerData(std::vector<render::LayerProperties> layerProprtList, QString currentFile);

    void setLayerData(layerStyle);

    void setModelIdexImage(QImage);

    void setItemChecked(bool);
    
signals:
    void signal_setLayerData(render::LayerProperties&);
    
public slots:
    void slot_treeItemChanged(QStandardItem *);

    void slot_treeDoubleClick(QModelIndex);

    void slot_activedModerIndex(QModelIndex);

    void slot_layerContextMenu(const QPoint&);

    void slot_showLayerControlWidget(bool);

    void slot_getLayerData(std::vector<render::LayerProperties>, QString);

    void slot_setBackgroundColor(QColor);

    void slot_setLineColor(QColor);

    void slot_setLineStyle(int);

    void slot_setLayerStyle(int);

    void slot_setTextColor(QColor);

    void slot_LineWidth_action();

    void slot_itemChecked(QStandardItem*);

    void slot_setLineWidth(int line_width);

private:
    QWidget* layerToolBar;
    PushButton *layerButton;
    QAction *layerAction;
    QMenu *layerMenu;
    QAction *loadAction;
    QAction *saveAction;
    QAction *reassignAction;
    PushButton *SaveButton;
    QMenu *SaveMenu;
    QAction *QuickloadAction1;
    QAction *QuickloadAction2;
    QAction *QuickloadAction3;
    QAction *QuicksaveAction1;
    QAction *QuicksaveAction2;
    QAction *QuicksaveAction3;
    Commbox *layerCombox;
    PushButton *linkButton;
    PushButton *LayerControlButton;

    QHBoxLayout *TreeHLayout;
    QTreeView *layerTree;

    LayerTreeModel *layerTreeModel;
    LayerTreeItem *rootFileItem;
    Delegate *treeDelegate;
    QAction *linewihthAction1;
    QAction *linewihthAction2;
    QAction *linewihthAction3;
    QAction *linewihthAction4;

    QAction *none;
    QAction *dashdotdot;
    QAction *solid;
    QAction *dash;
    QAction *dot;

    QWidget *LayerControlWidget;

    Commbox *LineStyleCommbox;
    Commbox *LineWidthCommbox;
    Commbox *TransparencyCommbox;

    PushButton *ButtonOn;
    PushButton *ButtonExpose;
    PushButton *ButtonSelectable;
    PushButton *ButtonText;
    PushButton *ButtonNage;
    PushButton *ButtonCut;

    QModelIndex * activeModelIndex;
    int m_active_model_index;
    render::LayerMetaData layerData;
    render::Pattern pattern;
    QVector <layerstyle> m_layer_style_vector;
    QVector <render::LayerProperties> m_layer_property_vector;
    render::LayerProperties m_LayerProperty;

    QPixmap pixmap;
};
}
#endif // LayerWidget_H
