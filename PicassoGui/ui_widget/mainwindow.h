#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QEvent>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QStringList>
#include <QString>
#include <QObject>
#include <QDebug>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QBoxLayout>
#include <QGridLayout>
#include <QCoreApplication>
#include <QLabel>
#include <QTreeView>
#include <QMap>
#include <QPoint>
#include <QScrollArea>
#include <QColorDialog>
#include <iostream>
#include <QSettings>
#include <QFile>
#include <QStringList>
#include <QCloseEvent>

#include "deftools/defcontrols.h"
#include "deftools/cmessagebox.h"
#include "deftools/global.h"
#include "checklist.h"
#include "defgroup.h"
#include "drawwidget.h"
#include "defectswidget.h"
#include "fileprojectwidget.h"
#include "layerwidget.h"
#include "choosefiledialog.h"
#include "rtsconfigdialog.h"
#include "semimagedialog.h"
#include "chipeditdialog.h"
#include "scaleframe.h"
#include "render_frame.h"
#include "../renderer/render_layer_metadata.h"
namespace UI{

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initTitleBar();

    void initMenubar();

    void initStatebar();

    void initDockWidget();
    
    void initToolbar();

    void initPaintTab();

    void initCheckList();

    void initDefGroup();

    void initConfigFile();

    void init_fileProject_widget();

    void init_fileProject_layerTree();

    void init_fileProject_workSpace();

    void initConnection();

    void initStyle();

    void saveOpenHistory(QString);
    void getOpenHistory();

    void readSettingConfig();

    void writeSettingConfig();

    void addHistoryAction(QString);

    void centerWidget_boundingSignal();

    void DefectReview();
    void RTSSetup();
    void RunRTS();
    void GaugeChecker();
    void SEMImageHandler();
    void ChipPlacementEditor();

protected:
    void closeEvent(QCloseEvent *);

signals:
    void signal_readDB(QString);

    void signal_addFile(QString);

    void signal_defgroupUpdata(QModelIndex *);

    void signal_defectsUpdata(QModelIndex *);

    void signal_setPenWidth(QString);

    void signal_setPaintStyle(Global::PaintStyle);

    void signal_close();

    void signal_getLayerData(std::vector<render::LayerProperties>, QString);

private slots:
    //Menu Action
    void slot_openFile();

    void slot_saveFile();

    void slot_closeFile();

    void slot_openREV();

    void slot_undo();

    void slot_drawPoint(const QModelIndex &);

    void slot_openDB(QString);

    void slot_addFile(QString);

    void slot_click_fileItem(QModelIndex);

    void slot_closePaintTab(int);

    void slot_updataXY(double, double);

    void slot_showDefGroup(QModelIndex, int);

    void slot_showDefects(QModelIndex, int);

    void slot_showState(QString);

    void slot_changePenWidth(QString);

    void ShowColorDialog();

    void slot_mouseAction();

    void slot_addHistoryAction();

    void slot_penAction();

    void slot_rulerAction();

    void slot_updataDistance(double);

    void slot_AddonActions();

    void slot_close();

    void slot_showScaleAxis(bool);

    void slot_setPosAction();

    void slot_setPosButton();

    void slot_setLayerData(render::LayerProperties&);

    void slot_refreshAction();

    void slot_moveCenter(int, int);
private:
    DockWidget *fileDockWidget;

    FileProjectWidget *fileWidget;

    DockWidget *layerDockWidget;

    LayerWidget *layerwidget;

    DockWidget *workspaceDockWidget;

    DockWidget *checkListDockWidget;

    DockWidget *logDockWidget;

    DockWidget *broserDockWidget;

    DockWidget *defGroupDockWidget;

    DockWidget *defectsDockWidget;

    QScrollArea *scrollArea;

    TabWidget *paintTab;

    DefGroup *defgroup;

    DefectsWidget *defectswidget;

    QString currentFile;

    QString DbPath;

    QTreeView *layerTree;

    QStandardItemModel *layerTreeModel;

    DrawWidget *paintWidget;
    QVector <DrawWidget *>paintWidget_vector;

    ScaleFrame *scaleFrame;
    QVector <ScaleFrame *>scaleFrame_vector;

    render::RenderFrame* renderFrame;
    QVector <render::RenderFrame*>renderFrame_vector;


    CheckList *checklistWidget;

    QVector <QModelIndex> modelIdexList;

    Global::PaintStyle paintstyle;

    bool _isCreatPaintWidget;

    std::vector<render::LayerProperties> layerPropertyList;

    render::LayerMetaData LayerData;

    //stateBar
    QLabel *currposLable_x;
    QLabel *currposLable_xNum;
    QLabel *currposLable_y;
    QLabel *currposLable_yNum;
    QLabel *distanceLable;
    QLabel *distanceLableNum;
    QLabel *stateLable;

    PushButton *colorBtn;
    PushButton *clearBtn;
    Commbox *penWidthCombox;

    QAction *penAction;
    QAction *rulerAction;
    QAction *mouseAction;

    bool isShowAxis;

    QWidget *setPosWidget;
    QLabel *setPosX_label;
    QLineEdit *setPosX_lineEdit;
    QLabel *setPosY_label;
    QLineEdit *setPosY_lineEdit;
    QPushButton *setPos_foundPushButton;
    QPushButton *setPos_colsePushButton;

    QMenu *rencentOpen_menu;
    QString configFile_path;
    QStringList historyFileList;
};
}
#endif // MAINWINDOW_H
