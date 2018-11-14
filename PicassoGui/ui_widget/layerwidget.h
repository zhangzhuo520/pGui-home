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

#include "menuwidget.h"
#include "../renderer/render_layer_metadata.h"
#include "../renderer/render_layer_properties.h"
#include "../renderer/render_pattern.h"

namespace UI{
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

    void updataLayerData(std::vector<render::LayerProperties> layerProprtList, QString currentFile);
    
signals:
    
public slots:
    void slot_treeItemChanged(QStandardItem *);

    void slot_treeDoubleClick(QModelIndex);

    void slot_activedModerIndex(QModelIndex);

    void slot_layerContextMenu(const QPoint&);

    void slot_showLayerControlWidget(bool);

    void slot_addLayerData(std::vector<render::LayerProperties>, QString);

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

    QStandardItemModel *layerTreeModel;

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

    QVector<render::LayerProperties> QlayerPropertyVctor;
    render::LayerMetaData layerData;
    render::Pattern pattern;
    QBitmap bitmap;
    QPixmap pixmap;
    QImage image;
};
}
#endif // LayerWidget_H
