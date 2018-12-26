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
#include "ui_tab_paintwidget.h"

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
    bool isCavseExist(QString);
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
    void singal_append_job(QString);
    void signal_addFile(QString);
    void signal_defgroupUpdata(QModelIndex *);
    void signal_defectsUpdata(QModelIndex *);
    void signal_setPenWidth(QString);
    void signal_setPenColor(const QColor &);
    void signal_setPaintStyle(Global::PaintStyle);
    void signal_getLayerData(render::RenderFrame*, QString);
    void signal_close_job(QString);

private slots:
    //Menu Action
    void slot_openFile();
    void slot_saveFile();
    void slot_closeFile(QString);
    void slot_openREV();
    void slot_undo();

    void slot_drawPoint(const QModelIndex &);
    void slot_open_job(QString);

    void slot_addFile(QString);
    void slot_creat_canvas(QModelIndex);
    void slot_close_paintwidget(int);
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

    void slot_close_database_widget(int);

    void slot_currentTab_changed(int);
    void slot_zoom_in();
    void slot_zoom_out();
    void slot_refresh();
    void slot_zoom_fit();
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

    void show_checklist(QString);
    void close_checklist_job(QString);
    void delete_checklist_job(QString);

    bool tab_is_job_or_osa(QString);

    DockWidget *fileDockWidget;
    FileProjectWidget *fileWidget;
    DockWidget *layerDockWidget;
    LayerWidget *layerwidget;
    DockWidget *workspaceDockWidget;
    DockWidget *checkListDockWidget;
    DockWidget *logDockWidget;
    DockWidget *broserDockWidget;
    QVector <DockWidget *> m_defgroupdockwidget_vector;
    QVector <DockWidget *> m_defectsdockwidget_vector;
    QFileDialog *m_file_dialog;
    QFileDialog *m_dir_dialog;

    TabPaintWidget *m_paint_tabwidget;
    PaintToolbar *m_paint_toolbar;
    QWidget * m_center_widget;

    QVector <DefGroup *> m_defgroup_vector;
    QVector <DefectsWidget *> m_defectswidget_vector;

    int m_current_tabid;
    QString m_current_filename;
    QString m_database_path;
    QTreeView *layerTree;

    CheckList *checklistWidget;
    Global::PaintStyle paintstyle;


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

    bool m_axis_show;

    QDialog *m_setpos_dialog;
    QLabel *m_pos_label;
    QLabel *m_pos_unit_label;
    QLineEdit *m_pos_lineeidt;

    QPushButton *m_setpos_okbutton;
    QPushButton *m_setpos_cancelbutton;

    QMenu *rencentOpen_menu;
    QString configFile_path;
    QStringList historyFileList;
    QString m_prep_dir;

    QStringList m_checklist_file_list;
};
}
#endif // MAINWINDOW_H
