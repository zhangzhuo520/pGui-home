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
#include "deftools/iconhelper.h"
#include "deftools/global.h"
#include "ui_checklist.h"
#include "ui_defgroup.h"
#include "ui_defects_widget.h"
#include "ui_fileproject_widget.h"
#include "ui_layer_widget.h"
#include "ui_choosefile_dialog.h"
#include "ui_rtsconfig_dialog.h"
#include "ui_semimage_dialog.h"
#include "ui_chipedit_dialog.h"
#include "ui_scale_frame.h"
#include "ui_paint_toolbar.h"

namespace ui{

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void saveOpenHistory(QString);

    void getOpenHistory();

    void readSettingConfig();

    void writeSettingConfig();

    void addHistoryAction(QString);

    void centerWidget_boundingSignal(int);

    bool isCavseExist(int);

    void showCoordinate();

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

    void signal_setPenColor(const QColor &);

    void signal_setPaintStyle(Global::PaintStyle);

    void signal_getLayerData(render::RenderFrame*, QString);

private slots:
    //Menu Action
    void slot_openFile();

    void slot_saveFile();

    void slot_closeFile();

    void slot_openREV();

    void slot_undo();

    void slot_drawPoint(const QModelIndex &);

    void slot_show_checklist(QString);

    void slot_addFile(QString);

    void slot_creat_canvas(QModelIndex);

    void slot_closePaintTab(int);

    void slot_updateXY(double, double);

    void slot_showDefGroup(QModelIndex, int);

    void slot_showDefects(QModelIndex, int);

    void slot_showState(QString);

    void slot_changePenWidth(QString);

    void ShowColorDialog();

    void slot_mouseAction();

    void slot_addHistoryAction();

    void slot_penAction();

    void slot_rulerAction();

    void slot_updateDistance(double);

    void slot_AddonActions();

    void slot_showScaleAxis(bool);

    void slot_setPosAction();

    void slot_setPosButton();

//    void slot_refreshAction();

    void slot_currentTab_changed(int);

    void slot_zoom_in();

    void slot_zoom_out();

    void slot_refrush();

private:
    void initTitleBar();

    void initMenubar();

    void initStatebar();

    void initDockWidget();

    void initToolbar();

    void initPaintTab();

    void initCheckList();

    void initDefGroup();

    void initConfigDir();

    void initPointer();

    void initPrepDir();

    void init_fileProject_widget();

    void init_fileProject_layerTree();

    void init_fileProject_workSpace();

    void initConnection();

    void initStyle();

    void open_database(QString);

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
    QFileDialog *m_file_dialog;

    TabWidget *paintTab;
    PaintToolbar *m_paint_toolbar;
    QWidget * m_center_widget;
    DefGroup *defgroup;
    DefectsWidget *defectswidget;

    int m_current_tabid;
    QString m_current_filename;
    QString DbPath;
    QTreeView *layerTree;

    QVector <ScaleFrame *>m_scaleFrame_vector;

    CheckList *checklistWidget;
    Global::PaintStyle paintstyle;

    bool _isCreatPaintWidget;


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

    QWidget *m_setpos_widget;
    QLabel *setPosX_label;
    QLineEdit *setPosX_lineEdit;
    QLabel *setPosY_label;
    QLineEdit *setPosY_lineEdit;
    QPushButton *setPos_foundPushButton;
    QPushButton *setPos_colsePushButton;

    QMenu *rencentOpen_menu;
    QString configFile_path;
    QStringList historyFileList;
    QString m_prep_dir;
};
}
#endif // MAINWINDOW_H
