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
#include <QApplication>
#include <QLayout>
#include <string>
#include <sstream>

#include "deftools/showcpumemory.h"
#include "deftools/defcontrols.h"
#include "deftools/cmessagebox.h"
#include "deftools/iconhelper.h"
#include "deftools/global.h"
#include "deftools/datastruct.h"
#include "deftools/datatable.h"
#include "ui_fileproject_widget.h"
#include "ui_measurepoint.h"
namespace ui{
class RtsConfigDialog;
class RtsReviewDialog;
class RtsCurve;
class RtsRunProcess;
class CheckList;
class DefGroup;
class DefectsWidget;
class FileProjectWidget;
class LayerWidget;
class ScaleFrame;
class PaintToolbar;
class GaugeTable;
class RtsImageParsing;
class RtsFileDialog;
class LogWidget;
class TabPaintWidget;
class Navigator;
class RtsManager;
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
    void writeTestingConfig();
    void addHistoryAction(QString);
    void centerWidget_boundingSignal(int);
    void showCoordinate();
    void DefectReview();
    void RTSSetup();
    void RunRTS();
    void GaugeChecker();
    void SEMImageHandler();
    void ChipPlacementEditor();
    void set_canvas_pos_range(const QString&, const QString&, const QString&);

    void enable_rts_image(QString &);
    RtsManager *rts_manager();

protected:
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *);

signals:
    void signal_append_job(const QString&);
    void signal_addFile(QString, FileType ,bool);
    void signal_defgroupUpdata(QModelIndex *);
    void signal_defectsUpdata(QModelIndex *);
    void signal_setPenWidth(QString);
    void signal_setPenColor(const QColor &);
    void signal_set_paint_tool(Global::PaintStyle);
//    void signal_getLayerData(render::RenderFrame*, QString);
    void signal_set_background_color(QColor);
    void signal_paint_cutline(Global::RtsCutLineAngle, QVariant);

private slots:
    //Menu Action
    void slot_openFile();
    void slot_saveFile();
    void slot_close_job_file(QString);
    void slot_openjob();
    void slot_open_gauge();
    void slot_coverage_job();
    void slot_open_coverage_job(QString);
    void slot_undo();

    void slot_drawPoint(const QModelIndex &);
    void slot_draw_gaugeline(const QModelIndex &);
    void slot_clear_gauge();
    void slot_open_job(QString);
    void slot_read_gauge(QString);

    void slot_rts_setting();
    void slot_rts_running();
    void slot_rts_file_dialog();

    void slot_addFile(QString);
    void slot_create_canvas(const FileInfo&);
    void slot_create_overlay_canvas(const FileInfo& );
    void slot_close_paintwidget(int);
    void slot_updateXY(double, double);
    void slot_showDefGroup(QModelIndex, int);
    void slot_showDefects(QModelIndex, int);
    void slot_showState(QString);
    void slot_changePenWidth(QString);
    void slot_set_pen_color();
    void slot_mouseAction();
    void slot_addHistoryAction();
    void slot_penAction();
    void slot_rulerAction();
    void slot_updateDistance(QString);
    void slot_AddonActions();
    void slot_showScaleAxis(bool);
    void slot_setPosAction();
    void slot_setPosButton();
//    void slot_setWindowMaxSizeAction();
//    void slot_setWindowMaxSizeButton();

    void slot_close_database_widget(int);
    void slot_close_defects(int);
    void slot_close_defgroup(int);

    void slot_currentTab_changed(int);
    void slot_zoom_in();
    void slot_zoom_out();
    void slot_refresh();
    void slot_zoom_fit();

    void slot_layout_view_changed(render::RenderFrame*);
    void slot_update_layername_list(const QStringList&);


    void slot_rts_finished(QVector <RtsReviewInfo>);
    void slot_rts_run_error(const QString &);
    void slot_update_canvas_pos();

    void slot_append_file(int);
    void slot_select_append_file();

    void slot_change_background_color();

    void slot_pos_jump(QModelIndex);

    void slot_rts_set_pos(QString, QString, QString);
    void slot_paint_cutline(Global::RtsCutLineAngle, QVariant);
    void slot_clear_rts_image(int);
    void slot_close_rts_file(int);
    void slot_update_cutline_table();
    void slot_update_cutline_paint(const QList <LineData *>&);

private:
    void init_titlebar();
    void init_menubar();
    void init_statebar();
    void init_dockwidget();
    void init_toolbar();
    void init_paint_tab();
    void init_checklist();
    void init_gaugetable();
    void init_navigator();
    void init_postable();
    void init_defgroup();
    void init_config_dir();
    void init_prep_dir();

    void init_fileProject_widget();
    void init_fileProject_layerTree();
    void init_fileProject_workSpace();
    void init_log_widget();
    void init_cpu_memory();

    void init_rtssetup_dialog();
    void init_setpos_dialog();
    void init_select_file_dialog();
    void init_connection();
    void init_style();

    void show_gauge_table(QStringList);
    void show_checklist(const QString&);
    bool defgroup_exist(QString);
    bool defectswidget_exist(QString);

    void add_file(const QString&, const FileType &, const bool &);

    void update_rts_job_commbox(const QStringList &);

    DockWidget *fileDockWidget;
    FileProjectWidget *fileWidget;
    DockWidget *layerDockWidget;
    LayerWidget *layerwidget;
    DockWidget *workspaceDockWidget;
    DockWidget *checkListDockWidget;
    DockWidget *logDockWidget;
    DockWidget *m_gauge_dockWidget;
    DockWidget *m_pos_dockwidget;
    DockWidget *m_navigator_dockwidget;
    Navigator *m_navigator_widget;
    QVector <DockWidget *> m_defgroupdockwidget_vector;
    QVector <DockWidget *> m_defectsdockwidget_vector;

    TabPaintWidget *m_paint_tabwidget;
    PaintToolbar *m_paint_toolbar;
    QWidget * m_center_widget;
    QScrollArea *m_scrollarea_bar;
    RtsConfigDialog *m_rtssetup_dialog;
    RtsReviewDialog *m_rtsrecview_dialog;


    int m_current_tabid;
    QString m_current_filename;
    QString m_database_path;
    QString m_database_name;
    QTreeView *layerTree;
    LogWidget *logwidget;
    GaugeTable *m_gauge_table;
    DataTable *m_pos_table;

    CheckList *m_checklist_widget;
    Global::PaintStyle paintstyle;

    //stateBar
    QLabel *currposLable_x;
    QLabel *currposLable_xNum;
    QLabel *currposLable_y;
    QLabel *currposLable_yNum;
    QLabel *distanceLable;
    QLabel *distanceLableNum;
    QLabel *stateLable;

    PushButton *bgColorBtn;
    PushButton *penColorBtn;
    PushButton *clearBtn;
    Commbox *penWidthCombox;

    QAction *penAction;
    QAction *rulerAction;
    QAction *mouseAction;

    bool m_axis_show;

    QDialog *m_setpos_dialog;
    QLabel *m_pos_label;
    QLabel *m_pos_unit_label;
    HistoryLineEdit *m_pos_lineedit;
    QLabel *m_pos_view_range_label;
    QLabel *m_pos_view_range_unit_label;
    QLineEdit *m_pos_view_range_edit;

//    QDialog *m_setwindow_dialog;
//    QLabel *m_window_label;
//    QLabel *m_window_unit_label;
//    QLineEdit *m_window_lineedit;

    QToolBar *RtsToolBar;
    QAction *RtsRunAction;

    QPushButton *m_setpos_okbutton;
    QPushButton *m_setpos_cancelbutton;

    QPushButton *m_setwindow_okbutton;
    QPushButton *m_setwindow_cancelbutton;

    QMenu *rencentOpen_menu;
    QString configFile_path;
    QStringList historyFileList;
    QString m_prep_dir;

    QStringList m_defgroup_name_list;
    QStringList m_open_job_list;

    ShowCPUMemory *m_show_cpumemory;

    QProgressIndicator *m_indicator;
    RtsFileDialog *m_rts_file_dialog;
    RtsManager *m_rts_manager;

    QDialog* m_select_file_dialog;
    QLabel* m_file_label;
    QComboBox* m_file_box;
    QPushButton* m_select_file_okbutton;
    QPushButton* m_select_file_cancelbutton;
};
}
#endif // MAINWINDOW_H
