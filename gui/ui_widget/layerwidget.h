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
#include "menuwidget.h"
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
    
signals:
    
public slots:
    void slot_treeItemChanged(QStandardItem *);

    void slot_layerUpdata(QString);

    void slot_layerContextMenu(const QPoint&);

    void slot_showLayerControlWidget(bool);

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
};

#endif // LayerWidget_H
