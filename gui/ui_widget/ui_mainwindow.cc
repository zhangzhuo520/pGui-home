#include "ui_mainwindow.h"
#include <QPlainTextEdit>

#include "rts_setup/ui_rtsconfig_dialog.h"
#include "rts_setup/ui_rtsreview_dialog.h"
#include "rts_setup/ui_rtscurve.h"
#include "rts_setup/rts_runprocess.h"
#include "ui_checklist.h"
#include "ui_defgroup.h"
#include "ui_defects_widget.h"
#include "ui_layer_widget.h"
#include "ui_choosefile_dialog.h"
#include "ui_semimage_dialog.h"
#include "ui_chipedit_dialog.h"
#include "ui_scale_frame.h"
#include "ui_paint_toolbar.h"
#include "ui_tab_paintwidget.h"
#include "ui_log_widget.h"
#include "ui_gaugetable.h"
#include "ui_navigator.h"
#include "fileparsing/rts_imageparsing.h"
#include "rts_setup/ui_rtsfile_dialog.h"
#include "rts_setup/rts_manager.h"

namespace ui {
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_current_tabid(-1)
{
    setObjectName("MainWindow");

    setWindowTitle("pGui");

    setDockNestingEnabled(true);

    //setWindowFlags(Qt::FramelessWindowHint);
    init_config_dir();

    init_menubar();

    init_toolbar();

    init_statebar();

    init_dockwidget();

    init_paint_tab();

    init_checklist();

    init_fileProject_widget();

    init_fileProject_layerTree();

    init_log_widget();

    init_gaugetable();

    init_navigator();

    init_rtssetup_dialog();

    init_setpos_dialog();

    init_select_file_dialog();

    init_postable();

//    init_maxwindow_dialog();

    init_connection();

    init_cpu_memory();

    init_prep_dir();

    init_style();

    readSettingConfig();

    writeTestingConfig();

    logger_widget("Software to start!");
}

MainWindow::~MainWindow()
{

}

/**
 * @brief MainWindow::init_menubar
 */
void MainWindow::init_menubar()
{
    QMenu *file_menu = menuBar()->addMenu("File");
    QAction *action = file_menu->addAction(QIcon(":/dfjy/images/open.png"),tr("Open"));
    action->setShortcuts(QKeySequence::Open);
    connect(action, SIGNAL(triggered()), this, SLOT(slot_openFile()));
    connect(action, SIGNAL(triggered()), this, SLOT(slot_saveFile()));
    file_menu->addSeparator();

    QAction *rencentOpen_action = new QAction("Recent Open", this);
    rencentOpen_menu = new QMenu(this);
    getOpenHistory();
    for (int i = 0; i < historyFileList.count(); i ++)
    {
        QAction *action = new QAction(historyFileList.at(i), rencentOpen_menu);
        action->setObjectName(historyFileList.at(i));
        rencentOpen_menu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(slot_addHistoryAction()));
    }
    rencentOpen_action->setMenu(rencentOpen_menu);
    file_menu->addAction(rencentOpen_action);
    file_menu->addAction(tr("&Quit"), this, SLOT(close()));

    QMenu *view_menu = menuBar()->addMenu(tr("View"));
    action = view_menu->addAction(tr("Undo"));
    connect(action, SIGNAL(triggered()), this, SLOT(slot_undo()));

    QMenu *layer_menu = menuBar()->addMenu(tr("Layer"));
    layer_menu->addAction("layer");
    QMenu *tool_menu = menuBar()->addMenu(tr("Tool"));
    tool_menu->addAction("tool");
    QMenu *option_menu = menuBar()->addMenu(tr("Option"));
    option_menu->addAction("Option");
    QMenu *window_menu = menuBar()->addMenu(tr("Window"));
    window_menu->addAction("window");
    QMenu *help_menu = menuBar()->addMenu(tr("Help"));
    help_menu->addAction("help");
//    QMenu *addon_menu = menuBar()->addMenu(tr("Addon"));
//    actionNameList.clear();
//    actionNameList << "Defect Review" << "RTS Setup" << "Run RTS"
//                   << "Gauge Checker" << "SEM Image Handler"
//                   << "Chip Placement Editor";
//    for(int i = 0; i < actionNameList.count(); i ++)
//    {
//        QAction *action = new QAction(actionNameList.at(i), addon_menu);
//        action->setObjectName(actionNameList.at(i));
//        addon_menu->addAction(action);
//        connect(action, SIGNAL(triggered()), this, SLOT(slot_AddonActions()));
//    }
}

/**
 * @brief MainWindow::init_statebar
 */
void MainWindow::init_statebar()
{
    stateLable = new QLabel("State: ");
    currposLable_x = new QLabel("X:");
    currposLable_y = new QLabel("Y:");
    distanceLable = new QLabel("Distance:");
    currposLable_xNum = new QLabel("00.000");
    currposLable_yNum = new QLabel("00.000");
    distanceLableNum = new QLabel("00.000");

    statusBar()->addWidget(stateLable, 10);
    statusBar()->addWidget(currposLable_x, 1);
    statusBar()->addWidget(currposLable_xNum, 3);
    statusBar()->addWidget(currposLable_y, 1);
    statusBar()->addWidget(currposLable_yNum, 3);
    statusBar()->addWidget(distanceLable, 1);
    statusBar()->addWidget(distanceLableNum, 3);
    statusBar()->setSizeGripEnabled(false);
}

/**
 * @brief MainWindow::init_dockwidget
 */
void MainWindow::init_dockwidget()
{
    static const struct Set {
        const char * name;
        uint flags;
        Qt::DockWidgetArea area;
    } sets [] = {
        { "File", 0, Qt::LeftDockWidgetArea },
        { "Layer", 0, Qt::LeftDockWidgetArea },
        { "pos", 0, Qt::LeftDockWidgetArea },
        { "CheckList", 0, Qt::BottomDockWidgetArea },
        { "Log", 0, Qt::LeftDockWidgetArea },
        { "Gauge", 0, Qt::RightDockWidgetArea },
        { "Navigator", 0, Qt::BottomDockWidgetArea }
    };

    fileDockWidget = new DockWidget(sets[0].name, this, Qt::WindowFlags(sets[0].flags));
    addDockWidget(sets[0].area, fileDockWidget);
    layerDockWidget = new DockWidget(sets[1].name, this, Qt::WindowFlags(sets[1].flags));
    addDockWidget(sets[1].area, layerDockWidget);
    m_pos_dockwidget = new DockWidget(sets[2].name, this, Qt::WindowFlags(sets[2].flags));
    addDockWidget(sets[2].area, m_pos_dockwidget);
    checkListDockWidget = new DockWidget(sets[3].name, this, Qt::WindowFlags(sets[3].flags));
    addDockWidget(sets[3].area, checkListDockWidget);
    logDockWidget = new DockWidget(sets[4].name, this, Qt::WindowFlags(sets[4].flags));
    addDockWidget(sets[4].area, logDockWidget);
    m_gauge_dockWidget = new DockWidget(sets[5].name, this, Qt::WindowFlags(sets[5].flags));
    addDockWidget(sets[5].area, m_gauge_dockWidget);
    m_navigator_dockwidget = new DockWidget(sets[6].name, this, Qt::WindowFlags(sets[5].flags));
    addDockWidget(sets[6].area, m_navigator_dockwidget);

    tabifyDockWidget(fileDockWidget,layerDockWidget);

//    tabifyDockWidget(logDockWidget, m_navigator_dockwidget);
//    tabifyDockWidget(fileDockWidget,workspaceDockWidget);
    fileDockWidget->raise();
    checkListDockWidget->hide();
//    m_gauge_dockWidget->setFloating(true);
    m_gauge_dockWidget->hide();
    m_pos_dockwidget->hide();
    m_navigator_dockwidget->hide();
}

/**
 * @brief MainWindow::init_toolbar
 */
void MainWindow::init_toolbar()
{
    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    QToolBar *FileToolBar = new QToolBar(this);
    FileToolBar->setWindowTitle(tr("File Actions"));
    addToolBar(FileToolBar);
    QAction *fileOpen;
    fileOpen = new QAction(QIcon(":/dfjy/images/open.png"), "Open" ,this);

    FileToolBar->addAction(fileOpen);
    connect(fileOpen, SIGNAL(triggered()), this, SLOT(slot_openFile()));

    QToolBar *MeasureToolbar = new QToolBar(this);
    MeasureToolbar->setWindowTitle(tr("Measure Actions"));
    addToolBar(MeasureToolbar);
    mouseAction = new QAction(QIcon(":/dfjy/images/mouse.png"), "Arrow" ,this);
    penAction = new QAction(QIcon(":/dfjy/images/pen.png"), "Pen" ,this);
    rulerAction = new QAction(QIcon(":/dfjy/images/ruler.png"), "Ruler" ,this);
    penAction->setCheckable(true);
    rulerAction->setCheckable(true);
    mouseAction->setCheckable(true);
    MeasureToolbar->addAction(mouseAction);
    MeasureToolbar->addAction(penAction);
    MeasureToolbar->addAction(rulerAction);

    connect(mouseAction, SIGNAL(triggered()), this, SLOT(slot_mouseAction()));
    connect(penAction, SIGNAL(triggered()), this, SLOT(slot_penAction()));
    connect(rulerAction, SIGNAL(triggered()), this, SLOT(slot_rulerAction()));

    QToolBar *screenContrlBar = new QToolBar(this);
    screenContrlBar->setWindowTitle(tr("screen Actions"));
    addToolBar(screenContrlBar);
    mouseAction->setChecked(true);

    QAction *zoomInAction = new QAction(QIcon(":/dfjy/images/zoomIn.png"),"zoomIn", this);
    QAction *zoomOutAction = new QAction(QIcon(":/dfjy/images/zoomOut.png"),"zoomout", this);
    QAction *refreshAction = new QAction(QIcon(":/dfjy/images/refresh.png"),"refresh", this);
    QAction *fitAction = new QAction(QIcon(":/dfjy/images/fit.png"),"fit", this);

    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(slot_zoom_in()));
    connect(zoomOutAction,SIGNAL(triggered()), this, SLOT(slot_zoom_out()));
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(slot_refresh()));
    connect(fitAction, SIGNAL(triggered()), this, SLOT(slot_zoom_fit()));

    screenContrlBar->addAction(zoomInAction);
    screenContrlBar->addAction(zoomOutAction);
    screenContrlBar->addAction(refreshAction);
    screenContrlBar->addAction(fitAction);

    QToolBar *RevToolBar = new QToolBar(this);
    RevToolBar->setWindowTitle(tr("REV Actions"));
    addToolBar(RevToolBar);

    QAction *revAction = new QAction(QIcon(":/dfjy/images/rev.png"),"rev", this);
    QAction *gaugeAction = new QAction(QIcon(":/dfjy/images/gauge_text.png"),"Gauge", this);

    RevToolBar->addAction(revAction);
    RevToolBar->addAction(gaugeAction);
    connect(revAction, SIGNAL(triggered()), this, SLOT(slot_openjob()));
    connect(gaugeAction, SIGNAL(triggered()), this, SLOT(slot_open_gauge()));

    QToolBar * backgroundBar = new QToolBar("this");
    backgroundBar->setWindowTitle("Background color bar");
    addToolBar(backgroundBar);

    bgColorBtn = new PushButton(this);
    QPixmap background_pixmap(20,20);
    background_pixmap.fill(Qt::white);
    bgColorBtn->setIcon(QIcon(background_pixmap));
    bgColorBtn->setToolTip(QString("Background color"));
    connect(bgColorBtn, SIGNAL(clicked()),  this, SLOT(slot_change_background_color()), Qt::UniqueConnection);
    backgroundBar->addWidget(bgColorBtn);

    QToolBar *PenBar = new QToolBar(this);
    PenBar->setWindowTitle(tr("Pen Actions"));
    addToolBar(PenBar);


    QLabel *penWidthLable = new QLabel("penWidth");

    penColorBtn = new PushButton(this);
    QPixmap pixmapColor(20, 20);
    pixmapColor.fill (Qt::black);
    penColorBtn->setIcon(QIcon(pixmapColor));
    penColorBtn->setToolTip(QString("Pen Color"));

    penWidthLable->setScaledContents(true);
    QPixmap pixmap(":/dfjy/images/lineWidth.png");
    pixmap = pixmap.scaled(20, 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    penWidthLable->setPixmap(pixmap);
    PenBar->addWidget(penWidthLable);
    penWidthCombox = new Commbox(this);
    connect(penWidthCombox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_changePenWidth(QString)));

    for(int i = 1; i < 10; i ++)
    {
        penWidthCombox->addItem(QString::number(i));
    }
    connect (penColorBtn, SIGNAL(clicked()), this, SLOT(slot_set_pen_color()), Qt::UniqueConnection);
    PenBar->addWidget(penWidthCombox);
    PenBar->addWidget(penColorBtn);

    QToolBar *ScaleToolBar = new QToolBar(this);
    ScaleToolBar->setWindowTitle(tr("Scale Actions"));
    addToolBar(ScaleToolBar);

    QAction *scaleAction = new QAction(QIcon(":/dfjy/images/scale.png"),"scale", this);
    scaleAction->setCheckable(true);
    connect(scaleAction, SIGNAL(toggled(bool)), this, SLOT(slot_showScaleAxis(bool)));
    ScaleToolBar->addAction(scaleAction);
    m_axis_show = false;

    QAction *setPosAction = new QAction(QIcon(":/dfjy/images/setpos.png"),"Set Position", this);
    connect(setPosAction, SIGNAL(triggered()), this, SLOT(slot_setPosAction()));
    ScaleToolBar->addAction(setPosAction);

//    QAction *setWindowMaxSizeAction = new QAction(QIcon(":/dfjy/images/setwindowsize.png"), "setWindowMaxSize", this);
//    connect(setWindowMaxSizeAction, SIGNAL(triggered()), this, SLOT(slot_setWindowMaxSizeAction()));
//    ScaleToolBar->addAction(setWindowMaxSizeAction);

    RtsToolBar = new QToolBar(this);
    RtsToolBar->setWindowTitle(tr("RTS Actions"));
    addToolBar(RtsToolBar);

    QAction *RtsSetAction = new QAction(QIcon(":/dfjy/images/rts.png"),"RtsSet", this);
    RtsToolBar->addAction(RtsSetAction);
    QAction *RtsFileEdit = new QAction(QIcon(":/dfjy/images/rts_file_edit.png"),"RtsFileEdit", this);
    RtsToolBar->addAction(RtsFileEdit);
    RtsRunAction = new QAction(QIcon(":/dfjy/images/run.png"),"RtsRun", this);
    RtsToolBar->addAction(RtsRunAction);

    connect(RtsSetAction, SIGNAL(triggered()), this, SLOT(slot_rts_setting()));
    connect(RtsRunAction, SIGNAL(triggered()), this, SLOT(slot_rts_running()));
    connect(RtsFileEdit, SIGNAL(triggered()), this, SLOT(slot_rts_file_dialog()));
}

/**
 * @brief MainWindow::initm_paint_tabwidget
 */
void MainWindow::init_paint_tab()
{
    m_center_widget = new QWidget(this);
    m_paint_tabwidget = new TabPaintWidget(this);
    m_paint_toolbar = new PaintToolbar(centralWidget());

    setCentralWidget(m_center_widget);
    m_paint_tabwidget->setObjectName("TabPaintWidget");
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    vLayout->addWidget(m_paint_tabwidget);
    vLayout->addWidget(m_paint_toolbar);

    m_center_widget->setLayout(vLayout);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    m_paint_tabwidget->setLayout(hLayout);

    connect(m_paint_tabwidget, SIGNAL(tabCloseRequested(int)), this, SLOT(slot_close_paintwidget(int)), Qt::UniqueConnection);   // TabWidget close
    connect(m_paint_tabwidget, SIGNAL(currentChanged(int)), this, SLOT(slot_currentTab_changed(int)));
    connect(m_paint_tabwidget, SIGNAL(signal_layout_view_changed(render::RenderFrame*)), this, SLOT(slot_layout_view_changed(render::RenderFrame*)));
    connect(this, SIGNAL(signal_set_paint_tool(Global::PaintStyle)), m_paint_toolbar, SLOT(slot_set_paint_tool(Global::PaintStyle)));
    connect(m_paint_toolbar, SIGNAL(signal_measure_table_click()), m_paint_tabwidget, SLOT(slot_show_measure_table()));

}

/**
 * @brief MainWindow::init_checklist
 */
void MainWindow::init_checklist()
{
    m_checklist_widget = new CheckList(width(), 200, this);
    m_checklist_widget->setMinimumHeight(0);
    checkListDockWidget->resize(100, 300);
    checkListDockWidget->setWidget(m_checklist_widget);

    connect(m_checklist_widget, SIGNAL(signal_close_database_widget(int)), this , SLOT(slot_close_database_widget(int)));
    connect(m_checklist_widget, SIGNAL(signal_coverage_job()), this, SLOT(slot_coverage_job()));
}

void MainWindow::init_gaugetable()
{
    m_gauge_table = new GaugeTable(this);
    m_gauge_dockWidget->setWidget(m_gauge_table);
    connect(m_gauge_table, SIGNAL(signal_send_gauge_data(QModelIndex)), this, SLOT(slot_draw_gaugeline(QModelIndex)));
    connect(m_gauge_dockWidget, SIGNAL(signal_clear_gauge()), this, SLOT(slot_clear_gauge()));
}

void MainWindow::init_navigator()
{
    m_navigator_widget = new Navigator(this);
    m_navigator_dockwidget->setWidget(m_navigator_widget);
}

void MainWindow::init_postable()
{
    QStringList headerlist;
    headerlist << "x" << "y" << "range";
    m_pos_table = new DataTable(m_pos_dockwidget, headerlist);
    m_pos_dockwidget->setWidget(m_pos_table);
    connect(m_pos_table, SIGNAL(pressed(QModelIndex)), this, SLOT(slot_pos_jump(QModelIndex)));
}

/**
 * @brief MainWindow::init_fileProject_fileTab
 */
void MainWindow::init_fileProject_widget()
{
    fileWidget = new FileProjectWidget(this);
    fileWidget->setMinimumHeight(0);
    fileDockWidget->setWidget(fileWidget);
    connect(this, SIGNAL(signal_addFile(QString, FileType, bool)), fileWidget, SLOT(slot_addFile(QString, FileType, bool)));
    connect(fileWidget, SIGNAL(signal_create_canvas(const FileInfo&)), this, SLOT(slot_create_canvas(const FileInfo&)));
    connect(fileWidget, SIGNAL(signal_create_overlay_canvas(const FileInfo&)), this, SLOT(slot_create_overlay_canvas(const FileInfo&)));
    connect(fileWidget, SIGNAL(signal_append_file(int)), this, SLOT(slot_append_file(int)));
}

/**
 * @brief MainWindow::init_fileProject_layerTree
 */
void MainWindow::init_fileProject_layerTree()
{
    layerwidget = new LayerWidget(this);
    layerDockWidget->setWidget(layerwidget);
    connect(layerwidget, SIGNAL(signal_update_layername_list(QStringList)), this, SLOT(slot_update_layername_list(QStringList)));
}

/**
 * @brief MainWindow::init_fileProject_workSpace
 */
void MainWindow::init_fileProject_workSpace()
{
//    broserDockWidget->setFixedHeight(50);
}

void MainWindow::init_log_widget()
{
    logwidget = new LogWidget();
    logDockWidget->setWidget(logwidget);
    connect(WidgetLogger::get_instance(), SIGNAL(signal_append_measasge(QString)), logwidget, SLOT(slot_append_log(QString)));
}

void MainWindow::init_cpu_memory()
{
    m_show_cpumemory = new ShowCPUMemory(this);
}

void MainWindow::init_rtssetup_dialog()
{
    m_rtssetup_dialog = new RtsConfigDialog(this);
    m_rtssetup_dialog->setGeometry(200, 200, 500, 600);
    connect(m_rtssetup_dialog, SIGNAL(signal_rts_finished(QVector <RtsReviewInfo>)), this, SLOT(slot_rts_finished(QVector <RtsReviewInfo>)));
    connect(m_rtssetup_dialog, SIGNAL(signal_rtsprocess_error(QString)), this, SLOT(slot_rts_run_error(QString)));
    m_rtssetup_dialog->hide();

    connect(m_rtssetup_dialog, SIGNAL(signal_get_current_canvaspos()), this, SLOT(slot_update_canvas_pos()));

    m_rtsrecview_dialog = new RtsReviewDialog(this);
    m_rtsrecview_dialog->setGeometry(200, 200, 400, 600);
    m_rtsrecview_dialog->hide();
    connect(m_rtsrecview_dialog, SIGNAL(signal_set_pos_range(QString,QString,QString)), this, SLOT(slot_rts_set_pos(QString,QString,QString)));
    connect(m_rtsrecview_dialog, SIGNAL(signal_paint_cutline(Global::RtsCutLineAngle,QVariant)), this, SLOT(slot_paint_cutline(Global::RtsCutLineAngle, QVariant)));
    connect(m_rtsrecview_dialog, SIGNAL(signal_updata_cutline_list(const QList <LineData*>&)), this, SLOT(slot_update_cutline_paint(const QList <LineData*>&)));
    m_rts_file_dialog = new RtsFileDialog(this);

    m_rts_manager = new RtsManager;
    connect(m_rts_manager, SIGNAL(signal_clear_rts_image(int)), SLOT(slot_clear_rts_image(int)));
    connect(m_rts_manager, SIGNAL(signal_colse_rts_file(int)), SLOT(slot_close_rts_file(int)));
}

void MainWindow::init_setpos_dialog()
{
    m_setpos_dialog = new QDialog(this);
    m_setpos_dialog->setWindowTitle("Set Position");
    m_setpos_dialog->setGeometry(width() / 2, height() / 2, 300, 140);
    m_pos_label = new QLabel("Pos(x, y):", m_setpos_dialog);
    m_pos_label->setGeometry(30, 30, 65, 25);
    m_pos_lineedit = new HistoryLineEdit(m_setpos_dialog);
    m_pos_lineedit->setToolTip("Enter a position as (x, y) in unit um");
    m_pos_lineedit->setGeometry(100, 30, 150, 25);
    m_pos_lineedit->setText(",");
    m_pos_unit_label = new QLabel("um", m_setpos_dialog);
    m_pos_unit_label->setGeometry(255, 30, 30, 25);

    m_pos_view_range_label = new QLabel("view range:", m_setpos_dialog);
    m_pos_view_range_label->setGeometry(30, 60, 65, 25);
    m_pos_view_range_edit = new QLineEdit(m_setpos_dialog);
    m_pos_view_range_edit->setGeometry(100, 60, 150, 25);
    m_pos_view_range_edit->setText("10");
    m_pos_view_range_edit->setCursorPosition(0);
    m_pos_view_range_unit_label = new QLabel("um", m_setpos_dialog);
    m_pos_view_range_unit_label->setGeometry(255, 60, 30, 25);

    m_setpos_okbutton = new QPushButton("OK", m_setpos_dialog);
    m_setpos_okbutton->setGeometry(150, 90, 60, 30);
    connect(m_setpos_okbutton, SIGNAL(clicked()), this, SLOT(slot_setPosButton()));
    m_setpos_cancelbutton = new QPushButton("Cancel", m_setpos_dialog);
    m_setpos_cancelbutton->setGeometry(220, 90, 60, 30);
    connect(m_setpos_cancelbutton, SIGNAL(clicked()), m_setpos_dialog, SLOT(close()));
    m_setpos_dialog->hide();
}

void MainWindow::init_select_file_dialog()
{
    m_select_file_dialog = new QDialog(this);
    m_select_file_dialog->setWindowTitle("Append file");
    m_select_file_dialog->setGeometry(width() / 2, height() / 2, 300, 120);
    m_file_label = new QLabel("Select file:", m_select_file_dialog);
    m_file_label->setGeometry(30, 30, 65, 25);
    m_file_box = new QComboBox(m_select_file_dialog);
    m_file_box->setGeometry(100, 30, 180, 25);

    m_select_file_okbutton = new QPushButton("OK", m_select_file_dialog);
    m_select_file_cancelbutton = new QPushButton("Cancel", m_select_file_dialog);
    m_select_file_okbutton->setGeometry(150, 70, 60, 30);
    m_select_file_cancelbutton->setGeometry(220, 70, 60, 30);

    connect(m_select_file_okbutton, SIGNAL(clicked()), this, SLOT(slot_select_append_file()));
    connect(m_select_file_cancelbutton, SIGNAL(clicked()), m_select_file_dialog, SLOT(close()));
    m_select_file_dialog->hide();
}

/**
 * @brief MainWindow::init_connection
 */
void MainWindow::init_connection()
{
    connect(this, SIGNAL(signal_append_job(const QString&)), m_checklist_widget, SLOT(slot_add_job(const QString&)));
//    connect(this, SIGNAL(signal_close_job(QString)), m_checklist_widget, SLOT(slot_close_job(QString)));
    connect(m_checklist_widget, SIGNAL(signal_close_job(QString)), this, SLOT(slot_close_job_file(QString)));  //checkList close job;
    connect(m_checklist_widget, SIGNAL(signal_showDefGroup(QModelIndex, int)), this ,SLOT(slot_showDefGroup(QModelIndex, int)));
    connect(fileWidget, SIGNAL(signal_openFile()), this, SLOT(slot_openFile()));
    connect(m_paint_tabwidget, SIGNAL(signal_current_frameinfo(FrameInfo)), m_checklist_widget, SLOT(slot_update_job(FrameInfo)));
}

/**
 * @brief MainWindow::init_style
 */
void MainWindow::init_style()
{
    IconHelper styleHelper;
    styleHelper.setStyle(":/dfjy/images/style.qss");
}

void MainWindow::show_gauge_table(QStringList AllDateList)
{
    QString HeaderList = AllDateList.at(0);
    m_gauge_table->set_header_list(HeaderList);
    AllDateList.removeAt(0);
    m_gauge_table->set_data_list(AllDateList);
    m_gauge_dockWidget->show();
}

void MainWindow::DefectReview()
{
}

void MainWindow::RTSSetup()
{
    QStringList list = layerwidget->get_all_layername();
    m_rtssetup_dialog->set_layername_list(list);
    m_rtssetup_dialog->show();
}

void MainWindow::RunRTS()
{
    m_rtssetup_dialog->start();
    setCursor(Qt::WaitCursor);
    RtsRunAction->setEnabled(false);
}

void MainWindow::GaugeChecker()
{
    ChooseFileDialog* chooseDialog = new ChooseFileDialog(this);
    chooseDialog->setWindowTitle("Choose File");
    chooseDialog->resize(400, 200);
    chooseDialog->show();
}

void MainWindow::SEMImageHandler()
{
    if(1)
    {
        SemImageDialog *semDialog = new SemImageDialog(this);
        semDialog->resize(400, 200);
        semDialog->show();
    }
}

void MainWindow::ChipPlacementEditor()
{
    chipEditDialog *chipDialog = new chipEditDialog(this);
    chipDialog->resize(500, 600);
    chipDialog->show();
}

void MainWindow::set_canvas_pos_range(const QString &xData, const QString &yData, const QString &rangeData)
{
    m_paint_tabwidget->get_scaleframe(m_current_tabid)->set_center_point(xData.toDouble(), yData.toDouble(), rangeData.toDouble());
}

void MainWindow::enable_rts_image(QString& image_path)
{
    render::RenderFrame* renderFrame = m_paint_tabwidget->get_scaleframe(m_current_tabid)->getRenderFrame();
    renderFrame->disble_all_rts_image();
    renderFrame->enable_rts_image(image_path);
}

RtsManager *MainWindow::rts_manager()
{
    return m_rts_manager;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    writeSettingConfig();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
//    m_indicator->move(width() / 2, height() / 2);
}

/**
 * @brief MainWindow::slot_showState
 * @param msg
 */
void MainWindow::slot_showState(QString msg)
{
    stateLable->setText("State:" +msg);
}

void MainWindow::slot_changePenWidth(QString penWidth)
{
    emit signal_setPenWidth(penWidth);
}

void MainWindow::slot_set_pen_color()
{
    QColor color = QColorDialog::getColor();

    if (color.isValid ())
    {
        QPixmap p(20, 20);
        p.fill (color);
        penColorBtn->setIcon (QIcon(p));
        emit signal_setPenColor(color);
    }
}

void MainWindow::slot_addHistoryAction()
{
    QString name = sender()->objectName();
    slot_addFile(name);
}

void MainWindow::slot_mouseAction()
{
    mouseAction->setChecked(true);
    penAction->setChecked(false);
    rulerAction->setChecked(false);
    paintstyle = Global::Normal;
    emit signal_set_paint_tool(paintstyle);
}

void MainWindow::slot_penAction()
{
    mouseAction->setChecked(false);
    penAction->setChecked(true);
    rulerAction->setChecked(false);
    paintstyle = Global::Mark;
    emit signal_set_paint_tool(paintstyle);
}

void MainWindow::slot_rulerAction()
{
    mouseAction->setChecked(false);
    penAction->setChecked(false);
    rulerAction->setChecked(true);
    paintstyle = Global::Measrue;
    emit signal_set_paint_tool(paintstyle);
}

void MainWindow::slot_updateDistance(QString length)
{
    distanceLableNum->setText(length);
}

void MainWindow::slot_AddonActions()
{
    QString actionName = sender()->objectName();
    QStringList actionNameList;
    actionNameList<< "Defect Review" << "RTS Setup" << "Run RTS"
                   << "Gauge Checker" << "SEM Image Handler"
                   << "Chip Placement Editor";
    for(int i = 0; i < actionNameList.count(); i ++)
    {
        if (actionName == actionNameList.at(i))
            switch (i)
            {
            case 0:
                DefectReview();
                break;
            case 1:
                RTSSetup();
                break;
            case 2:
                RunRTS();
                break;
            case 3:
                GaugeChecker();
                break;
            case 4:
                SEMImageHandler();
                break;
            case 5:
                ChipPlacementEditor();
                break;
            default:
                break;
            }
    }
}

void MainWindow::slot_showScaleAxis(bool isShow)
{
     m_axis_show = isShow;

     if (m_paint_tabwidget->count() == 0)
     {
         return;
     }

     for (int i = 0; i < m_paint_tabwidget->count(); i ++)
     {
         showCoordinate();
     }
}

void MainWindow::slot_setPosAction()
{
    if ((m_current_tabid < m_paint_tabwidget->count()) && (m_current_tabid != -1) && (m_paint_tabwidget->count() > 0))
    {
        double limit = m_paint_tabwidget->get_scaleframe(m_current_tabid)->get_view_range();
        std::ostringstream ss;
        ss << limit;
        m_pos_view_range_edit->setText(QString::fromStdString(ss.str()));
    }
    else
    {
        m_pos_view_range_edit->setText("120.0");
    }
    m_setpos_dialog->show();
}


void MainWindow::slot_setPosButton()
{
    QStringList list =  m_pos_lineedit->text().split(',');
    if (list.count() != 2)
    {
        showWarning(this, "Warning", "Input is Error!");
        return;
    }
    else if (list.at(0).isEmpty() || list.at(1).isEmpty())
    {
        showWarning(this, "Warning", "Input is empty!");
        return;
    }

    m_pos_lineedit->addItem(m_pos_lineedit->text());
    QString view_range = m_pos_view_range_edit->text();

    if ((m_current_tabid < m_paint_tabwidget->count()) && (m_paint_tabwidget->count() > 0))
    {
        QString data = list.at(0) + "##" + list.at(1) + "##" + view_range;
        m_pos_table->append_data(data);
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->set_center_point(list.at(0).toDouble(), list.at(1).toDouble(), view_range.toDouble());
        m_setpos_dialog->close();
    }
    else
    {
        showWarning(this, "Warning", "Not open the canvas!");
    }

    if (m_pos_dockwidget->isHidden())
    {
        m_pos_dockwidget->show();
    }
}

void MainWindow::slot_close_database_widget(int job_number)
{
    QString defgroup = "Job" + QString::number(job_number) +"_category";
    DockWidget *defgroup_widget = findChild < DockWidget * >(defgroup);
    int defgroup_index = m_defgroupdockwidget_vector.indexOf(defgroup_widget);
    if (defgroup_index >= 0 && defgroup_index < m_defgroupdockwidget_vector.count())
    {
        defgroup_widget->close();
    }

    QString defect = "Job" + QString::number(job_number) +"_defects";
    DockWidget *defect_widget = findChild < DockWidget * >(defect);

    int defect_index = m_defectsdockwidget_vector.indexOf(defect_widget);
    if (defect_index >= 0 && defect_index < m_defectsdockwidget_vector.count())
    {
        defect_widget->close();
    }
}

void MainWindow::slot_close_defgroup(int index)
{
    QString defgroup = "Job" + QString::number(index) +"_category";
    DockWidget *defgroup_widget = findChild < DockWidget * >(defgroup);
    int defgroup_index = m_defgroupdockwidget_vector.indexOf(defgroup_widget);
    if (defgroup_index >= 0 && defgroup_index < m_defgroupdockwidget_vector.count())
    {
        m_defgroupdockwidget_vector.remove(defgroup_index);
        defgroup_widget = 0;
    }
}

void MainWindow::slot_close_defects(int index)
{
    QString defect = "Job" + QString::number(index) +"_defects";
    DockWidget *defect_widget = findChild < DockWidget * >(defect);

    int defect_index = m_defectsdockwidget_vector.indexOf(defect_widget);
    if (defect_index >= 0 && defect_index < m_defectsdockwidget_vector.count())
    {
        m_defectsdockwidget_vector.remove(defect_index);
        defect_widget = 0;
    }
}



void MainWindow::slot_currentTab_changed(int index)
{
    m_current_tabid = index;
    render::RenderFrame *renderFrame = NULL;
    if (index == -1)
    {
        m_paint_tabwidget->clear();
        renderFrame = NULL;
        return;
    }
    else
    {
        renderFrame = m_paint_tabwidget->get_scaleframe(m_current_tabid)->getRenderFrame();
    }
    layerwidget->getLayerData(renderFrame);
    m_paint_tabwidget->update_measuretable_data();
    ScaleFrame * frame = m_paint_tabwidget->get_scaleframe(m_current_tabid);
    m_checklist_widget->update_current_job(frame->frame_info());
    m_navigator_widget->update_source_frame(frame->getRenderFrame());
}

/**
 * @brief MainWindow::slot_closem_paint_tabwidget
 * @param index
 */
void MainWindow::slot_close_paintwidget(int index)
{
    ScaleFrame * frame = m_paint_tabwidget->get_scaleframe(index);
    QVector <FileInfo> all_filetype_vector;
    for (int i = 0; i < m_paint_tabwidget->count(); i ++)
    {
       FrameInfo info = m_paint_tabwidget->get_scaleframe(i)->frame_info();
       for (int j = 0; j < info.fileinfo_vector.count(); j ++)
       {
           all_filetype_vector.append(info.fileinfo_vector.at(j));
       }
    }

    if (index == m_paint_tabwidget->currentIndex())
    {
        m_checklist_widget->delete_job(frame->frame_info(), false, all_filetype_vector, true);
    }
    else
    {
        m_checklist_widget->delete_job(frame->frame_info(), false, all_filetype_vector, false);
    }
    m_paint_tabwidget->slot_close_tab(index);
}

void MainWindow::slot_zoom_in()
{
    if(m_paint_tabwidget->count() > 0)
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->zoom_in();
    }
}

void MainWindow::slot_zoom_out()
{
    if(m_paint_tabwidget->count() > 0)
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->zoom_out();
    }
}

void MainWindow::slot_refresh()
{
    if(m_paint_tabwidget->count() > 0)
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->refresh();
    }
}

void MainWindow::slot_zoom_fit()
{
    if(m_paint_tabwidget->count() > 0)
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->zoom_fit();
    }
}

/**
 * @brief MainWindow::slot_openFile
 */
void MainWindow::slot_openFile()
{
    QString file = QFileDialog::getOpenFileName(this, "Open Layout File", QDir::homePath(), tr("All layout files(*.oas *.OAS *.GDS *.gds)"));
    if (file.isEmpty())
    {
        return;
    }
    else
    {
        slot_addFile(file);
    }
}

/**
 * @brief MainWindow::slot_saveFile
 */
void MainWindow::slot_saveFile()
{
}

void MainWindow::slot_openjob()
{
    QString file = QFileDialog::getExistingDirectory(this, "Open Job", QDir::homePath());
    if (file.isEmpty())
    {
        return;
    }
    else
    {
        slot_open_job(file);
    }
}

void MainWindow::slot_open_gauge()
{
    QString file = QFileDialog::getOpenFileName(this, "Open Gauge", QDir::homePath(), tr("file (*.txt)"));
    if (file.isEmpty())
    {
        return;
    }
    else
    {
        slot_read_gauge(file);
    }
}

void MainWindow::slot_coverage_job()
{
    QString file = QFileDialog::getExistingDirectory(this, "Append Job", QDir::homePath());
    if (file.isEmpty())
    {
        return;
    }
    else
    {
        slot_open_coverage_job(file);
    }
}

void MainWindow::slot_open_coverage_job(QString dirName)
{
    QString oas_path = dirName + "/DefectFile.oas";
    if (fileWidget->is_file_exist(oas_path))
    {
        if (showWarning(this, "Waring", "GDS/Job already opened. Do you want to \nre-open it?", QMessageBox::StandardButtons(QMessageBox::No | QMessageBox::Ok))
                == QMessageBox::Ok )
        {
            fileWidget->delete_file(oas_path);
        }
        else
        {
            return;
        }
    }

    QDir dir(dirName);
    QFileInfoList flist = dir.entryInfoList();
    foreach(QFileInfo fileInfo, flist)
    {
        if(!fileInfo.isFile())
            continue;
        if (fileInfo.fileName() == "DefectFile.oas")
        {
            QString str = dirName + "/DefectFile.oas";
            add_file(str, FileType::job, true);
        }
    }
}

/**
 * @brief MainWindow::slot_undo
 */
void MainWindow::slot_undo()
{
}

void MainWindow::slot_drawPoint(const QModelIndex &index)
{
    double point_x = index.sibling(index.row(), 2).data().toDouble();
    double point_y = index.sibling(index.row(), 3).data().toDouble();
    QString Stringsize = index.sibling(index.row(), 1).data().toString();
    logger_file(QString::number(m_current_tabid) + "," + QString::number(point_x) + "," +  QString::number(point_y) + "," + Stringsize);
    m_paint_tabwidget->get_scaleframe(m_current_tabid)->drawDefectPoint(point_x, point_y, Stringsize);
}

void MainWindow::slot_draw_gaugeline(const QModelIndex &index)
{
    double start_point_x = 0;
    double start_point_y = 0;

    double end_point_x = 0;
    double end_point_y = 0;
    QString line_info = "";

    for (int i = 0; i < index.model()->columnCount(); i ++)
    {
        QString index_data = index.model()->headerData(i, Qt::Horizontal).toString();
        if (index_data == "startx")
        {
            start_point_x = index.sibling(index.row(), i).data().toDouble() / 1000;
        }
        if (index_data == "starty")
        {
            start_point_y = index.sibling(index.row(), i).data().toDouble() / 1000;
        }
        if (index_data == "endx")
        {
            end_point_x = index.sibling(index.row(), i).data().toDouble() / 1000;
        }
        if (index_data == "endy")
        {
            end_point_y = index.sibling(index.row(), i).data().toDouble() / 1000;
        }
        if (index_data == "gauge")
        {
            line_info = index.sibling(index.row(), i).data().toString();
        }
    }

    //move point
    if ((m_current_tabid < m_paint_tabwidget->count()) && (m_paint_tabwidget->count() > 0))
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->set_center_point((end_point_x + start_point_x) / 2, (end_point_y + start_point_y) / 2) ;
    }
    else
    {
        showWarning(this, "Warning", "Not open the canvas!");
        return;
    }

    //draw line
    m_paint_tabwidget->get_scaleframe(m_current_tabid)->draw_gauge_line(QPointF(start_point_x, start_point_y), QPointF(end_point_x, end_point_y), line_info);
}

void MainWindow::slot_clear_gauge()
{
    if ((m_current_tabid < m_paint_tabwidget->count()) && (m_paint_tabwidget->count() > 0))
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->slot_clear_gauge();
    }
    else
    {
        showWarning(this, "Warning", "Not open the canvas!");
        return;
    }
}

void MainWindow::slot_open_job(QString dirName)
{
    QString oas_path = dirName + "/DefectFile.oas";
    if (fileWidget->is_file_exist(oas_path))
    {
        if (showWarning(this, "Waring", "GDS/Job already opened. Do you want to \nre-open it?", QMessageBox::StandardButtons(QMessageBox::No | QMessageBox::Ok))
                == QMessageBox::Ok )
        {
            fileWidget->delete_file(oas_path);
        }
        else
        {
            return;
        }
    }

    QDir dir(dirName);
    QFileInfoList flist = dir.entryInfoList();
    foreach(QFileInfo fileInfo, flist)
    {
        if(!fileInfo.isFile())
            continue;
        if (fileInfo.fileName() == "DefectFile.oas")
        {
            QString str = dirName + "/DefectFile.oas";
            add_file(str, FileType::job ,false);
            m_open_job_list.append(dirName);
            update_rts_job_commbox(m_open_job_list);
        }
        if(fileInfo.suffix() == "sqlite")
        {
            m_database_path = dirName + "/" + fileInfo.fileName();
            m_database_name = fileInfo.fileName();
            logger_widget("Open Database: " + m_database_path);
        }
    }
}

void MainWindow::slot_read_gauge(QString FilePath)
{
    QStringList m_text_list;
    QFile TextFile(FilePath);
    if (!TextFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "File open faild";
    }
    while(!TextFile.atEnd()) {
        QByteArray line = TextFile.readLine();
        QString str(line);
        m_text_list.append(str);
    }

    show_gauge_table(m_text_list);
}

void MainWindow::slot_rts_setting()
{
    RTSSetup();
}

void MainWindow::slot_rts_running()
{
    RunRTS();
}

void MainWindow::slot_rts_file_dialog()
{
    m_rts_file_dialog->resize(800, 400);
    m_rts_file_dialog->show();
}

void MainWindow::show_checklist(const QString& job_path)
{
    emit signal_append_job(job_path);
    checkListDockWidget->show();
}

bool MainWindow::defgroup_exist(QString title)
{
    for (int i = 0; i < m_defgroupdockwidget_vector.count(); i ++)
    {
        if (title == m_defgroupdockwidget_vector.at(i)->objectName())
        {
            return true;
        }
    }
    return false;
}

bool MainWindow::defectswidget_exist(QString title)
{
    for (int i = 0; i < m_defectsdockwidget_vector.count(); i ++)
    {
        if (title == m_defectsdockwidget_vector.at(i)->objectName())
        {
            return true;
        }
    }
    return false;
}

void MainWindow::add_file(const QString& filePath, const FileType& file_type, const bool & isOverLay)
{
    if (fileWidget->is_file_exist(filePath))
    {
        if (showWarning(this, "Warning", "GDS/Job already opened. Do you want to \nre-open it?", QMessageBox::StandardButtons(QMessageBox::No | QMessageBox::Ok))
                == QMessageBox::Ok )
        {
            fileWidget->delete_file(filePath);
        }
        else
        {
            return;
        }
    }

    double filesize = QString::number(QFile(filePath).size()).toDouble() /  1024.00 / 1024.00;

    if (0 == filesize)
    {
        logger_widget(QString("%1 not exist!").arg(filePath));
        return;
    }
    logger_widget("Open file : " + filePath + ",   Size : " +  QString::number(filesize) + "MB");
    logger_widget(m_show_cpumemory->GetCPU() + ", " + m_show_cpumemory->GetMemory());
    emit signal_addFile(filePath, file_type, isOverLay);
    saveOpenHistory(filePath);
    addHistoryAction(filePath);
}

void MainWindow::update_rts_job_commbox(const QStringList & list)
{
    m_rtssetup_dialog->update_job_commbox(list);
}

/**
 * @brief MainWindow::slot_addFile
 * @param filePath
 */
void MainWindow::slot_addFile(QString filePath)
{
    FileType file_type = FileType::file;
    bool is_overlay = false;
    add_file(filePath, file_type, is_overlay);
}

/**
 * @brief MainWindow::slot_create_canvas
 * @param index
 */
void MainWindow::slot_create_canvas(const FileInfo& file_info)
{
    QString active_filename = QString::fromStdString(file_info.layout_view->file_name());

    if (!file_info.layout_view->has_single_view())
    {
        ScaleFrame* frame = m_paint_tabwidget->creat_canvas();
        std::string prep_dir = m_prep_dir.toStdString();
        file_info.layout_view->load_into_frame(frame->getRenderFrame(), prep_dir);
        m_paint_tabwidget->append_canvas();
        centerWidget_boundingSignal(m_paint_tabwidget->count() - 1);

        FrameInfo frame_info;
        frame_info.fileinfo_vector.append(file_info);
        frame_info.file_type = file_info.file_type;
        frame->set_frame_info(frame_info);

        if(file_info.file_type == FileType::job)
        {
            QString path = active_filename.left(active_filename.size() - 15);
            show_checklist(path);
        }
    }
    else
    {
        m_paint_tabwidget->set_active_widget(file_info.layout_view->single_view());
    }
    showCoordinate();
    m_navigator_widget->update_source_frame(m_paint_tabwidget->get_scaleframe(m_current_tabid)->getRenderFrame());
}

void MainWindow::slot_create_overlay_canvas(const FileInfo& file_info)
{
    if (m_current_tabid == -1)
    {
        showWarning(this, "Warning", "Not open canvas !");
    }

    ScaleFrame *frame = m_paint_tabwidget->get_scaleframe(m_current_tabid);
    frame->frame_info().fileinfo_vector.append(file_info);

    try
    {
        file_info.layout_view->attach(frame->getRenderFrame(), m_prep_dir.toStdString(), true);
    }
    catch(const render::append_error& e)
    {
        showWarning(this, "Warning", QString::fromStdString(e.what()));
        return ;
    }
    catch(const render::append_exception& e)
    {
        QString canvas_name = frame->get_file_name();
        QString warning_message = QString("%1 \n The canvas name is: %2").arg(QString::fromStdString(e.what())).arg(canvas_name);
        showWarning(this, "Warning",warning_message);
        return ;
    }
    QString file_name(frame->get_file_name());
    m_paint_tabwidget->setTabText(m_current_tabid, file_name);

    if (file_info.file_type == FileType::job)
    {
        QString job_path = QString::fromStdString(file_info.layout_view->file_name()).left(QString::fromStdString(file_info.layout_view->file_name()).count() - 15);
        show_checklist(job_path);
    }

    layerwidget->getLayerData(frame->getRenderFrame());
    showCoordinate();
}

void MainWindow::slot_updateXY(double x, double y)
{
    currposLable_xNum->setText(QString::number(x));
    currposLable_yNum->setText(QString::number(y));
}

/**
 * @brief MainWindow::slot_showDefGroup
 * @param index
 */
void MainWindow::slot_showDefGroup(QModelIndex index, int current_defgroup_index)
{
    QModelIndex tableIdIndex = index.sibling(index.row(), 9);
    QString widget_title = "";
    widget_title = "Job" + QString::number(current_defgroup_index) +"_category";
    if (!tableIdIndex.isValid())
    {
        logger_file("Category index error! ");
        return;
    }
    QModelIndex temp_index = index;
    while (!temp_index.parent().data().toString().isEmpty()){
        temp_index = temp_index.parent();
    }
    QStringList database_path_list = temp_index.data().toString().split(":");
    if (database_path_list.count() < 2)
    {
        logger_console << QString("database path is error : 1%").arg(temp_index.data().toString());
    }
    m_database_path = database_path_list.at(1) + "/" + m_database_name;
    logger_file(widget_title);
    if(!tableIdIndex.data().toString().isEmpty() && (!defgroup_exist(widget_title)))
    {
        DockWidget *defGroupDockWidget = new DockWidget(widget_title, this, 0);
        addDockWidget(Qt::RightDockWidgetArea, defGroupDockWidget);
        DefGroup *defgroup = new DefGroup(defGroupDockWidget, m_database_path, &index, current_defgroup_index);
        defGroupDockWidget->setWidget(defgroup);
        m_defgroupdockwidget_vector.append(defGroupDockWidget);
        connect(defgroup, SIGNAL(signal_showDefects(QModelIndex, int)), this, SLOT(slot_showDefects(QModelIndex, int)));
        connect(defGroupDockWidget, SIGNAL(signal_close_database_widget(int)), this , SLOT(slot_close_defgroup(int)));
    }
    else
    {
        if (m_defgroupdockwidget_vector.count() < 1)
        {
            return;
        }

        for (int i = 0; i < m_defgroupdockwidget_vector.count(); i ++)
        {
            if (widget_title == m_defgroupdockwidget_vector.at(i)->objectName())
            {
                logger_file(i);
                logger_file(m_defgroupdockwidget_vector.count());
                m_defgroupdockwidget_vector.at(i)->show();
                DefGroup *defgroup = static_cast <DefGroup *> (m_defgroupdockwidget_vector.at(i)->widget());
                defgroup->updata_all_data(&index);
            }
        }
    }
}

/**
 * @brief MainWindow::slot_showDefects
 * @param index
 * @param jobIndex
 */
void MainWindow::slot_showDefects(QModelIndex index, int jobIndex)
{
    if (!index.isValid())
    {
        return;
    }

    QString widget_title = "Job" + QString::number(jobIndex) +"_defects";

    if(!index.data().toString().isEmpty() && (!defectswidget_exist(widget_title)))
    {
        DockWidget* defectsDockWidget = new DockWidget(widget_title , this, 0);
        addDockWidget(Qt::RightDockWidgetArea, defectsDockWidget);
        m_defectsdockwidget_vector.append(defectsDockWidget);
        DefectsWidget *defectswidget = new DefectsWidget(defectsDockWidget, m_database_path, &index, jobIndex);
        defectsDockWidget->setWidget(defectswidget);
        connect(defectswidget->getTableView(), SIGNAL(clicked(const QModelIndex&)), this,  SLOT(slot_drawPoint(const QModelIndex &)));
        connect(defectsDockWidget, SIGNAL(signal_close_database_widget(int)), this , SLOT(slot_close_defects(int)));
    }
    else
    {
        if (m_defectsdockwidget_vector.count() < 1)
        {
            return;
        }

        for (int i = 0; i < m_defectsdockwidget_vector.count(); i ++)
        {
            if (widget_title == m_defectsdockwidget_vector.at(i)->objectName())
            {
                m_defectsdockwidget_vector.at(i)->show();
                DefectsWidget *defectswidget = static_cast <DefectsWidget *> (m_defectsdockwidget_vector.at(i)->widget());
                defectswidget->update_all_data(&index);
            }
        }
    }
}

void MainWindow::init_config_dir()
{
    configFile_path = QDir::homePath() + "/.pangen_gui" + "/pgui_config";
    QDir dir(configFile_path);

    if (!dir.exists())
    {
        if(!dir.mkpath(configFile_path))
        {
            qDebug() << "make config_dir error !";
            return;
        }
    }
}

void MainWindow::init_prep_dir()
{
    m_prep_dir = QDir::homePath()+ "/.pangen_gui" + "/pgui_prep";
    QDir dir(m_prep_dir);
    if (!dir.exists())
    {
         if(!dir.mkpath(m_prep_dir))
        {
            qDebug() << "make prep_dir error !";
            return;
        }
    }
}

void MainWindow::saveOpenHistory(QString history)
{
    QString filePath = configFile_path + "/openFileHistory.txt";
    QString str;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    QTextStream out(&file);

    if (historyFileList.count() > 4)
    {
        historyFileList.insert(0, history);
    }
    else
    {
        historyFileList.append(history);
    }

    for (int i = 0; i < historyFileList.count(); i ++)
    {
        if (historyFileList.count() > 6)
        {
            historyFileList.removeAt(historyFileList.count() - 1);
        }
    }

    for (int i = 0; i < historyFileList.count(); i ++)
    {
        str = str + historyFileList.at(i) + "###";
    }

    out << str;
    file.flush();
    file.close();
}

void  MainWindow::getOpenHistory()
{
    QFile file(configFile_path + "/openFileHistory.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    historyFileList.clear();
    QTextStream out(&file);
    historyFileList =  out.readAll().split("###");
    historyFileList.removeAt(historyFileList.count() - 1);
    file.close();
}

void MainWindow::readSettingConfig()
{
    QSettings configSet(configFile_path + "/configSize.ini", QSettings::IniFormat);
    configSet.clear();
    QSize mainwindow_size = configSet.value("Grometry", QVariant(QSize(600,600))).toSize();
    QPoint point = configSet.value("Pos", QVariant(QPoint(0,0))).toPoint();
    resize(mainwindow_size.width(), mainwindow_size.height());
    move(point);
}

void MainWindow::writeSettingConfig()
{
    QSettings configSet(configFile_path + "/configSize.ini", QSettings::IniFormat);
    configSet.setValue("Grometry", size());
    configSet.setValue("Pos", pos());
}

void MainWindow::writeTestingConfig()
{
    QString filePath = configFile_path + "/autoTest.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    QTextStream out(&file);

    QList<QAction*> menubar_actions = menuBar()->actions();
    for(int i = 0 ; i < menubar_actions.count(); i++)
    {
        QRect rect = menuBar()->actionGeometry(menubar_actions.at(i));
        QString text = menubar_actions.at(i)->text();
        out <<  QString("Menu->%1 action ").arg(text) << QString("left: %1 bot:%2 right:%3 top:%4").arg(rect.left()).arg(rect.bottom()).arg(rect.right()).arg(rect.top());
        out << endl;
    }

    QList<QToolBar*> toolbars= findChildren<QToolBar*>();
    for(int i = 0; i < toolbars.count(); i++)
    {
        QList<QAction*> actions_per_toolbar = toolbars.at(i)->actions();
        QRect rect = toolbars.at(i)->geometry();
        for(int j = 0; j < actions_per_toolbar.count(); j++)
        {
            QString text = actions_per_toolbar.at(j)->text();
            if(text.isEmpty())
            {
                continue;
            }
            int left = rect.left() + rect.width() / actions_per_toolbar.count() * j;
            int right = left +  rect.width() / actions_per_toolbar.count();
            int bottom = rect.bottom();
            int top = rect.top();

            out <<  QString("ToolBar->%1 action ").arg(text) << QString("left: %1 bot:%2 right:%3 top:%4").arg(left).arg(bottom).arg(right).arg(top);
            out << endl;
        }
    }

    QRect rect = fileDockWidget->geometry();
    QString text = fileDockWidget->windowTitle();
    out << QString("DockWidget->%1 widget:").arg(text) << QString("left: %1 bot:%2 right:%3 top:%4").arg(rect.left()).arg(rect.bottom()).arg(rect.right()).arg(rect.top());
    out << endl;

    rect = layerDockWidget->geometry();
    text = layerDockWidget->windowTitle();
    out << QString("DockWidget->%1 widget:").arg(text) << QString("left: %1 bot:%2 right:%3 top:%4").arg(rect.left()).arg(rect.bottom()).arg(rect.right()).arg(rect.top());
    out << endl;

    rect = logDockWidget->geometry();
    text = logDockWidget->windowTitle();
    out << QString("DockWidget->%1 widget:").arg(text) << QString("left: %1 bot:%2 right:%3 top:%4").arg(rect.left()).arg(rect.bottom()).arg(rect.right()).arg(rect.top());
    out << endl;

    file.flush();
    file.close();
}

void MainWindow::addHistoryAction(QString filename)
{
    QAction *action = new QAction(filename, rencentOpen_menu);
    action->setObjectName(filename);
    rencentOpen_menu->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(slot_addHistoryAction()));
}

void MainWindow::centerWidget_boundingSignal(int index)
{
    connect(this, SIGNAL(signal_setPenWidth(QString)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_pen_width(QString)));
    connect(this, SIGNAL(signal_setPenColor(const QColor&)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_pen_color(const QColor&)));
    connect(this, SIGNAL(signal_paint_cutline(Global::RtsCutLineAngle,QVariant)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_paint_cutline(Global::RtsCutLineAngle,QVariant)));
    connect(m_paint_tabwidget->get_scaleframe(index), SIGNAL(signal_updateDistance(QString)), this, SLOT(slot_updateDistance(QString)));
    connect(m_paint_tabwidget->get_scaleframe(index), SIGNAL(signal_pos_updated(double, double)), this, SLOT(slot_updateXY(double, double)));
    connect(m_paint_toolbar, SIGNAL(signal_setSnapFlag(Global::SnapFLag)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_snapfalg(Global::SnapFLag)));
    connect(m_paint_tabwidget->get_scaleframe(index), SIGNAL(signal_updata_cutline_table()), this, SLOT(slot_update_cutline_table()));
    connect(this, SIGNAL(signal_set_background_color(QColor)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_background_color(QColor)));
    connect(m_paint_toolbar, SIGNAL(signal_setPaintStyle(Global::PaintTool)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_painter_style(Global::PaintTool)));
    connect(m_paint_toolbar, SIGNAL(signal_all_clear()), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_clear_all()));
    connect(m_paint_toolbar, SIGNAL(signal_measure_line_clear()), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_clear_measureline()));
    connect(m_paint_toolbar, SIGNAL(signal_mark_clear()), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_clear_mark_point()));
    emit signal_setPenWidth(penWidthCombox->currentText());
    m_paint_toolbar->updata_toolbar();
}

void MainWindow::showCoordinate()
{
    m_paint_tabwidget->get_scaleframe(m_current_tabid)->is_show_axis(m_axis_show);
}

static bool conflicts(const render::RenderFrame* frame, const QVector<render::RenderFrame*>& frame_list)
{
    for(int i = 0; i < frame_list.count(); i++)
    {
        if(frame != frame_list[i])
        {
            std::vector<render::LayoutView*> tmp1 = frame->get_layout_views_list();
            std::vector<render::LayoutView*> tmp2 = frame_list[i]->get_layout_views_list();
            std::sort(tmp1.begin(), tmp1.end());
            std::sort(tmp2.begin(), tmp2.end());
            if(tmp1.size() != tmp2.size())
            {
                continue;
            }

            bool result = true;
            for(size_t j = 0; j < tmp1.size(); j++)
            {
                if(tmp1[j] != tmp2[j])
                {
                    result = false;
                    break;
                }
            }

            if(result)
            {
                return result;
            }
        }
    }
    return false;
}

void MainWindow::slot_layout_view_changed(render::RenderFrame* frame)
{
    QVector <FileInfo> all_filetype_vector;
    for (int i = 0; i < m_paint_tabwidget->count(); i ++)
    {
       FrameInfo info = m_paint_tabwidget->get_scaleframe(i)->frame_info();
       for (int j = 0; j < info.fileinfo_vector.count(); j ++)
       {
           all_filetype_vector.append(info.fileinfo_vector.at(j));
       }
    }

    for(int i = 0; i < m_paint_tabwidget->count(); i++)
    {
        ScaleFrame* scale_frame = m_paint_tabwidget->get_scaleframe(i);
        if(scale_frame->getRenderFrame() == frame)
        {
            if(i == m_current_tabid)
            {
                layerwidget->getLayerData(frame);
            }

            if(frame->layout_views_size() == 0 || conflicts(frame, m_paint_tabwidget->get_render_frame_list()))
            {
                m_checklist_widget->delete_job(scale_frame->frame_info(), true, all_filetype_vector, true);
                m_paint_tabwidget->slot_close_tab(i);
            }
            else
            {
                m_checklist_widget->delete_job(scale_frame->frame_info(), true, all_filetype_vector, true);
                QString file_name = scale_frame->get_file_name();
                QStringList list = file_name.split("/", QString::SkipEmptyParts);
                QString abbr_file_name = list.back();
                m_paint_tabwidget->setTabText(i, abbr_file_name);
                QString tab_tool_tip;
                QVector<QString> result = scale_frame->get_file_name_list();
                if(result.count() == 1)
                {
                    tab_tool_tip.append(QString("Single:"));
                    tab_tool_tip.append(result.at(0));
                    m_paint_tabwidget->setTabToolTip(i, tab_tool_tip);
                }
                else
                {
                    tab_tool_tip.append(QString("Append:"));
                    for(int j = 0; j < result.count(); j++)
                    {
                        tab_tool_tip.append(result.at(j));
                        tab_tool_tip.append(QString("\n"));
                    }
                    m_paint_tabwidget->setTabToolTip(i, tab_tool_tip);
                }
            }
        }
    }
}

void MainWindow::slot_update_layername_list(const QStringList & list)
{
    m_rtssetup_dialog->set_layername_list(list);
}

void MainWindow::slot_rts_finished(QVector <RtsReviewInfo> InfoVector)
{
    QString data = QString::number(InfoVector[0].index) + "##" + InfoVector[0].x + "##" + InfoVector[0].y + "##" + InfoVector[0].range;
    m_rtsrecview_dialog->append_history_data(data);
    m_rtsrecview_dialog->rts_setup_clear();
    render::RenderFrame* renderFrame = m_paint_tabwidget->get_scaleframe(m_current_tabid)->getRenderFrame();
    for (int i = 0; i < InfoVector.count(); i ++)
    {
        FileType file_type = FileType::file;

        add_file(InfoVector[i].output_path, file_type, true);
        m_rtsrecview_dialog->append_setup_combox("Rts" + QString::number(i));
        QString tmp = InfoVector[i].output_path;
        int index = tmp.lastIndexOf('/');
        if(index == -1)
        {
            return ;
        }

        tmp = tmp.left(index) + "/images";
        QDir image_dir(tmp);

        if(!image_dir.exists())
        {
            return ;
        }

        QFileInfoList list = image_dir.entryInfoList();
        QStringList info_path_list;
        QStringList image_path_list;

        for(int i = 0; i < list.size(); i++)
        {
            QFileInfo fileInfo = list.at(i);
            if(fileInfo.suffix() == "data")
            {
                qDebug() << fileInfo.filePath();
                info_path_list.push_back(fileInfo.filePath());
            }

            if (fileInfo.suffix() == "png")
            {
                image_path_list.append(fileInfo.filePath());
            }

        }

        if(m_current_tabid == -1)
        {
            return ;
        }

        renderFrame->add_rts_image(info_path_list);
        RtsInfo rts_info;
        rts_info.set_file_path(InfoVector[i].output_path);
        rts_info.set_image_list(image_path_list);
        rts_info.set_rts_id(InfoVector[i].index);
        m_rts_manager->add_rts(rts_info);
    }

    renderFrame->disble_all_rts_image();
    m_rtsrecview_dialog->updata_image_setup();

    RtsRunAction->setEnabled(true);
    setCursor(Qt::ArrowCursor);
    m_rtsrecview_dialog->show();
}

void MainWindow::slot_rts_run_error(const QString & error)
{
    qDebug() << error;
    RtsRunAction->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void MainWindow::slot_update_canvas_pos()
{
    double left, right, bottom, top;
    m_paint_tabwidget->get_canvas_coord(&left, &right, &bottom, &top);
    m_rtssetup_dialog->set_canvas_pos(left, right, bottom, top);
}

void MainWindow::slot_append_file(int index)
{
    int rowCount = fileWidget->file_count();
    if(rowCount == 1)
    {
        showWarning(this, "Warning", "Only one file exists in the file manager.");
        return;
    }
    if (index < 0)
    {
        return;
    }

    render::LayoutView* from_lv = (*fileWidget->get_files_info_iter(index)).layout_view;
    render::RenderFrame* from_frame = from_lv->single_view();

    int active_index = -1;
    if(from_frame != 0)
    {
        for(int i = 0; i < (int)m_paint_tabwidget->count(); i++)
        {
            render::RenderFrame* frame = m_paint_tabwidget->get_scaleframe(i)->getRenderFrame();
            if( frame ==  from_frame)
            {
                active_index = i;
                break;
            }
        }

        if(active_index == -1)
        {
            showWarning(this, "Warning", "No canvas to select.");
            return ;
        }
    }
    int nums = m_file_box->count();
    for(int i = 0; i < nums; i++)
    {
        m_file_box->removeItem(0);
    }

    QFont font;
    QFontMetrics fm(font);
    int maxWidth = 0;

    for(int i = 0 ;i < (int)m_paint_tabwidget->count(); i++)
    {
        if(i != active_index)
        {
            QList<QVariant> list;
            list.append(QVariant(i));
            list.append(QVariant(index));
            QString filename = m_paint_tabwidget->get_scaleframe(i)->get_file_name();
            int width = fm.width(filename);
            if( maxWidth < width)
            {
                maxWidth = width;
            }
            m_file_box->addItem(filename, QVariant(list));
        }
    }

    m_select_file_dialog->show();
}

void MainWindow::slot_select_append_file()
{
    m_select_file_dialog->close();
    if (m_file_box->count() == 0)
    {
        return;
    }
    int index = m_file_box->currentIndex();
    QVariant var = m_file_box->itemData(index);
    QList<QVariant> list = var.toList();
    QVariant to_frame_data = list.at(0);
    QVariant from_lv_data = list.at(1);
    int to_frame_index = to_frame_data.toInt();
    int from_lv_index = from_lv_data.toInt();

    render::LayoutView* from_lv = (*(fileWidget->get_files_info_iter(from_lv_index))).layout_view;

    render::RenderFrame* frame = m_paint_tabwidget->get_scaleframe(to_frame_index)->getRenderFrame();

    if(frame->index_of_layout_views(from_lv) != -1)
    {
        showWarning(this, "Warning", "The selected view has included the file you want to append.");
        return;
    }
    else
    {   try
        {
            from_lv->attach(frame, "" , true);
        }
        catch(const render::append_error& e)
        {
            showWarning(this, "Warning", QString::fromStdString(e.what()));
        }
        catch(const render::append_exception& e)
        {
            QString canvas_name;
            for(int i = 0; i < m_paint_tabwidget->count(); i++)
            {
                ScaleFrame* scale_frame = m_paint_tabwidget->get_scaleframe(i);
                if(scale_frame->getRenderFrame() == e.m_frame)
                {
                    canvas_name =  scale_frame->get_file_name();
                }
            }
            QString warning_message = QString("%1 \n The canvas name is: %2").arg(QString::fromStdString(e.what())).arg(canvas_name);
            showWarning(this, "Warning",warning_message);
        }
    }
}

void MainWindow::slot_change_background_color()
{
    QColor color = QColorDialog::getColor();
    {
        if(color.isValid())
        {
            QPixmap p(20,20);
            p.fill(color);
            bgColorBtn->setIcon(QIcon(p));
            emit signal_set_background_color(color);
        }
    }
}

void MainWindow::slot_pos_jump(QModelIndex index)
{
    double x = index.sibling(index.row(), 0).data().toDouble();
    double y = index.sibling(index.row(), 1).data().toDouble();
    double range = index.sibling(index.row(), 2).data().toDouble();
    m_paint_tabwidget->get_scaleframe(m_current_tabid)->set_center_point(x, y, range);
}

void MainWindow::slot_rts_set_pos(QString x, QString y, QString range)
{
    set_canvas_pos_range(x, y, range);
}

void MainWindow::slot_paint_cutline(Global::RtsCutLineAngle angel_mode, QVariant data)
{
    emit signal_paint_cutline(angel_mode, data);
    penAction->setChecked(false);
    rulerAction->setChecked(false);
    mouseAction->setChecked(false);
    emit signal_set_paint_tool(Global::RtsCutLine);

}

void MainWindow::slot_clear_rts_image(int rts_id)
{
    Q_UNUSED(rts_id);
//    render::RenderFrame* renderFrame = m_paint_tabwidget->get_scaleframe(m_current_tabid)->getRenderFrame();
//    renderFrame->add_rts_image(info_path_list);
}

void MainWindow::slot_close_rts_file(int rts_id)
{
    QString file_path =  m_rts_manager->rts_info(rts_id).file_path();
    if (file_path.isEmpty())
    {
        logger_console << "close rts file error, file path is empty!";
    }

    fileWidget->delete_file(file_path);
}

void MainWindow::slot_update_cutline_table()
{
    if (m_paint_tabwidget->count() == 0)
    {
        return;
    }

    QList <LineData *> line_list = m_paint_tabwidget->get_scaleframe(m_paint_tabwidget->currentIndex())->get_cutline_list();
    m_rtsrecview_dialog->set_cutline_table(line_list);

}

void MainWindow::slot_update_cutline_paint(const QList <LineData *>& line_list)
{
    if (m_paint_tabwidget->count() == 0)
    {
        return;
    }

    m_paint_tabwidget->get_scaleframe(m_paint_tabwidget->currentIndex())->set_cutline_list(line_list);
}

void MainWindow::slot_close_job_file(QString db_path)
{
        QString file_name = db_path + "/DefectFile.oas";
        fileWidget->delete_file(file_name);
}
}

