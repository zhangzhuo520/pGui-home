#include "ui_mainwindow.h"
namespace ui {
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setObjectName("MainWindow");

    setWindowTitle("pGui");

    setDockNestingEnabled(true);

    //setWindowFlags(Qt::FramelessWindowHint);
    initConfigDir();

    initMenubar();

    initToolbar();

    initStatebar();

    initDockWidget();

    initPaintTab();

    initCheckList();

    init_fileProject_widget();

    init_fileProject_layerTree();

    init_log_widget();

    initGaugeTable();

    init_rtssetup_dialog();

    initConnection();

    init_cpu_memory();

    initPointer();

    initPrepDir();

    initStyle();

    readSettingConfig();

    writeTestingConfig();

    logger_widget("Software to start!");
}

MainWindow::~MainWindow()
{

}

/**
 * @brief MainWindow::initMenubar
 */
void MainWindow::initMenubar()
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
 * @brief MainWindow::initStatebar
 */
void MainWindow::initStatebar()
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
 * @brief MainWindow::initDockWidget
 */
void MainWindow::initDockWidget()
{
    static const struct Set {
        const char * name;
        uint flags;
        Qt::DockWidgetArea area;
    } sets [] = {
        { "File", 0, Qt::LeftDockWidgetArea },
        { "Layer", 0, Qt::LeftDockWidgetArea },
        { "Workspace", 0, Qt::BottomDockWidgetArea },
        { "CheckList", 0, Qt::BottomDockWidgetArea },
        { "Log", 0, Qt::LeftDockWidgetArea },
        { "Gauge", 0, Qt::RightDockWidgetArea }
    };

    fileDockWidget = new DockWidget(sets[0].name, this, Qt::WindowFlags(sets[0].flags));
    addDockWidget(sets[0].area, fileDockWidget);
    layerDockWidget = new DockWidget(sets[1].name, this, Qt::WindowFlags(sets[1].flags));
    addDockWidget(sets[1].area, layerDockWidget);
    //workspaceDockWidget = new DockWidget(sets[2].name, this, Qt::WindowFlags(sets[2].flags));
    //addDockWidget(sets[2].area, workspaceDockWidget);
    checkListDockWidget = new DockWidget(sets[3].name, this, Qt::WindowFlags(sets[3].flags));
    addDockWidget(sets[3].area, checkListDockWidget);
    logDockWidget = new DockWidget(sets[4].name, this, Qt::WindowFlags(sets[4].flags));
    addDockWidget(sets[4].area, logDockWidget);
    m_gauge_dockWidget = new DockWidget(sets[5].name, this, Qt::WindowFlags(sets[5].flags));
    addDockWidget(sets[5].area, m_gauge_dockWidget);

    tabifyDockWidget(fileDockWidget,layerDockWidget);
//    tabifyDockWidget(fileDockWidget,workspaceDockWidget);
    fileDockWidget->raise();
    checkListDockWidget->hide();
//    m_gauge_dockWidget->setFloating(true);
    m_gauge_dockWidget->hide();
}

/**
 * @brief MainWindow::initToolbar
 */
void MainWindow::initToolbar()
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
    connect(revAction, SIGNAL(triggered()), this, SLOT(slot_openREV()));
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

    QAction *setWindowMaxSizeAction = new QAction(QIcon(":/dfjy/images/setwindowsize.png"), "setWindowMaxSize", this);
    connect(setWindowMaxSizeAction, SIGNAL(triggered()), this, SLOT(slot_setWindowMaxSizeAction()));
    ScaleToolBar->addAction(setWindowMaxSizeAction);

    RtsToolBar = new QToolBar(this);
    RtsToolBar->setWindowTitle(tr("RTS Actions"));
    addToolBar(RtsToolBar);

    QAction *RtsSetAction = new QAction(QIcon(":/dfjy/images/rts.png"),"RtsSet", this);
    RtsToolBar->addAction(RtsSetAction);
    RtsRunAction = new QAction(QIcon(":/dfjy/images/run.png"),"RtsRun", this);
    RtsToolBar->addAction(RtsRunAction);
    connect(RtsSetAction, SIGNAL(triggered()), this, SLOT(slot_rts_setting()));
    connect(RtsRunAction, SIGNAL(triggered()), this, SLOT(slot_rts_running()));
}

/**
 * @brief MainWindow::initm_paint_tabwidget
 */
void MainWindow::initPaintTab()
{
    QWidget* p = centralWidget();
    if(p)
    {
        delete p;
    }
    m_current_tabid = -1;
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
 * @brief MainWindow::initCheckList
 */
void MainWindow::initCheckList()
{
    checklistWidget = new CheckList(width(), 200, this);
    checklistWidget->setMinimumHeight(0);
    checkListDockWidget->resize(100, 300);
    checkListDockWidget->setWidget(checklistWidget);

    connect(checklistWidget, SIGNAL(signal_close_database_widget(int)), this , SLOT(slot_close_database_widget(int)));
    connect(checklistWidget, SIGNAL(signal_coverage_job()), this, SLOT(slot_coverage_job()));
}

void MainWindow::initGaugeTable()
{
    m_gauge_table = new GaugeTable(this);
    m_gauge_dockWidget->setWidget(m_gauge_table);
    connect(m_gauge_table, SIGNAL(signal_send_gauge_data(QModelIndex)), this, SLOT(slot_draw_gaugeline(QModelIndex)));
    connect(m_gauge_dockWidget, SIGNAL(signal_clear_gauge()), this, SLOT(slot_clear_gauge()));
}

/**
 * @brief MainWindow::init_fileProject_fileTab
 */
void MainWindow::init_fileProject_widget()
{
    fileWidget = new FileProjectWidget(this);
    fileWidget->setMinimumHeight(0);
    fileDockWidget->setWidget(fileWidget);
    connect(this, SIGNAL(signal_addFile(QString, bool)), fileWidget, SLOT(slot_addFile(QString, bool)));
    connect(fileWidget, SIGNAL(signal_create_canvas(QModelIndex)), this, SLOT(slot_create_canvas(QModelIndex)));
    connect(fileWidget, SIGNAL(signal_create_overlay_canvas(QModelIndex)), this, SLOT(slot_create_overlay_canvas(QModelIndex)));
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
    m_rtssetup_dialog->setGeometry(200, 200, 400, 600);
    m_rtssetup_dialog->hide();

    connect(m_rtssetup_dialog, SIGNAL(signal_get_current_canvaspos()), this, SLOT(slot_update_canvas_pos()));

    m_rtsrecview_dialog = new RtsReviewDialog(this);
    m_rtsrecview_dialog->setGeometry(200, 200, 400, 600);
    m_rtsrecview_dialog->hide();

//    m_indicator = new QProgressIndicator(this);
//    m_indicator->setColor(Qt::black);
//    m_indicator->move(width() / 2, height() / 2);
    m_imagedata_parising = new RtsImageParsing(this);
    connect(m_imagedata_parising, SIGNAL(signal_parsing_finished()), this, SLOT(slot_rts_image_finished()));
}
/**
 * @brief MainWindow::initConnection
 */
void MainWindow::initConnection()
{
    connect(this, SIGNAL(signal_append_job(QString)), checklistWidget, SLOT(slot_add_job(QString)));
    connect(this, SIGNAL(signal_close_job(QString)), checklistWidget, SLOT(slot_close_job(QString)));
    connect(checklistWidget, SIGNAL(signal_close_job(QString)), this, SLOT(slot_close_file(QString)));  //checkList close job;
    connect(checklistWidget, SIGNAL(signal_showDefGroup(QModelIndex, int)), this ,SLOT(slot_showDefGroup(QModelIndex, int)));
    connect(fileWidget, SIGNAL(signal_openFile()), this, SLOT(slot_openFile()));
}

/**
 * @brief MainWindow::initStyle
 */
void MainWindow::initStyle()
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
    m_run_process = new RtsRunProcess(this);
    connect(m_run_process, SIGNAL(signal_rtsrun_finished()), this, SLOT(slot_rtsrun_finished()));
    m_run_process->run();
    RtsRunAction->setEnabled(false);
    setCursor(Qt::WaitCursor);
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
    if (m_setpos_dialog == NULL)
    {
        m_setpos_dialog = new QDialog(this);
        m_setpos_dialog->setWindowTitle("Set Position");
        m_setpos_dialog->setGeometry(width() / 2 - 150, height() / 2 - 150 , 300, 140);
        m_pos_label = new QLabel("Pos(x, y):", m_setpos_dialog);
        m_pos_label->setGeometry(30, 30, 65, 25);
        m_pos_lineedit = new QLineEdit(m_setpos_dialog);
        m_pos_lineedit->setToolTip("Enter a position as (x, y) in unit um");
        m_pos_lineedit-> setGeometry(100, 30, 150, 25);
        m_pos_lineedit->setText(",");
        m_pos_lineedit->setCursorPosition(0);
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
    }
    if ((m_current_tabid < m_paint_tabwidget->count()) && (m_paint_tabwidget->count() > 0))
    {
        double limit = m_paint_tabwidget->get_scaleframe(m_current_tabid)->get_view_range();
        std::ostringstream ss;
        ss << limit;
        m_pos_lineedit->setText(QString::fromStdString(ss.str()));
    }
    else
    {
        m_window_lineedit->setText("120.0");
    }

    m_setpos_dialog->show();
}

void MainWindow::slot_setWindowMaxSizeAction()
{
    if(m_setwindow_dialog == NULL)
    {
        m_setwindow_dialog = new QDialog(this);
        m_setwindow_dialog->setWindowTitle("set WindowMaxSize");
        m_setwindow_dialog->setGeometry(width() / 2 - 150, height() / 2 - 150, 300, 140);
        m_window_label = new QLabel("Size:", m_setwindow_dialog);
        m_window_label->setGeometry(30, 30, 70, 25);
        m_window_lineedit = new QLineEdit(m_setwindow_dialog);
        m_window_lineedit->setGeometry(100, 30, 150, 25);
        m_window_lineedit->setText("120.0");
        m_window_lineedit->setCursorPosition(0);
        m_window_unit_label = new QLabel("um", m_setwindow_dialog);
        m_window_unit_label->setGeometry(255, 30, 30, 25);
        m_setwindow_okbutton = new QPushButton("OK", m_setwindow_dialog);
        m_setwindow_okbutton->setGeometry(150, 90, 60, 30);
        connect(m_setwindow_okbutton, SIGNAL(clicked()), this, SLOT(slot_setWindowMaxSizeButton()));
        m_setwindow_cancelbutton = new QPushButton("Cancel", m_setwindow_dialog);
        m_setwindow_cancelbutton->setGeometry(220, 90, 60, 30);
        connect(m_setwindow_cancelbutton, SIGNAL(clicked()), m_setwindow_dialog, SLOT(close()));
    }
    m_setwindow_dialog->show();
}

void MainWindow::slot_setPosButton()
{
    QStringList list =  m_pos_lineedit->text().split(',');
    QString view_range = m_pos_view_range_edit->text();
    if (list.count() != 2)
    {
        return;
    }
    else
    {
        if ((m_current_tabid < m_paint_tabwidget->count()) && (m_paint_tabwidget->count() > 0))
        {
            m_paint_tabwidget->get_scaleframe(m_current_tabid)->set_center_point(list.at(0).toDouble(), list.at(1).toDouble(), view_range.toDouble());
            m_setpos_dialog->close();
        }
        else
        {
            showWarning(this, "Warning", "Not open the canvas!");
        }
    }
}

void MainWindow::slot_setWindowMaxSizeButton()
{
    double limit = m_window_lineedit->text().toDouble();

    if((m_current_tabid < m_paint_tabwidget->count()) && (m_paint_tabwidget->count() > 0))
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->set_window_max_size(limit);
        m_setwindow_dialog->close();
    }
    else
    {
        showWarning(this, "Warning", "Not open the canvas!");
    }
}

void MainWindow::slot_close_database_widget(int job_number)
{
    QString defgroup = "Job" + QString::number(job_number) +"_defGroup";
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

void MainWindow::slot_close_defects(int index)
{
    QString defect = "Job" + QString::number(index) +"_defects";
    DockWidget *defect_widget = findChild < DockWidget * >(defect);

    int defect_index = m_defectsdockwidget_vector.indexOf(defect_widget);
    if (defect_index >= 0 && defect_index < m_defectsdockwidget_vector.count())
    {
        m_defectsdockwidget_vector.remove(defect_index);
    }
}

void MainWindow::slot_close_defgroup(int index)
{
    QString defgroup = "Job" + QString::number(index) +"_defGroup";
    DockWidget *defgroup_widget = findChild < DockWidget * >(defgroup);
    int defgroup_index = m_defgroupdockwidget_vector.indexOf(defgroup_widget);

    if (defgroup_index >= 0 && defgroup_index < m_defgroupdockwidget_vector.count())
    {
        m_defgroupdockwidget_vector.remove(defgroup_index);
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
    }
    else
    {
        renderFrame = m_paint_tabwidget->get_scaleframe(m_current_tabid)->getRenderFrame();
    }
    layerwidget->getLayerData(renderFrame);
    m_paint_tabwidget->update_measuretable_data();
}

/**
 * @brief MainWindow::slot_closem_paint_tabwidget
 * @param index
 */
void MainWindow::slot_close_paintwidget(int index)
{
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
    if (m_file_dialog == NULL)
    {
        m_file_dialog = new QFileDialog(this);
    }
    m_file_dialog->setWindowModality(Qt::ApplicationModal);
    m_file_dialog->setWindowTitle(tr("Open Layout File"));
    m_file_dialog->setDirectory("/home/dfjy/workspace/job");
    m_file_dialog->setNameFilter(tr("All layout files(*.oas *.OAS *.GDS *.gds)"));
    connect(m_file_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_addFile(QString)), Qt::UniqueConnection);
    m_file_dialog->setFileMode(QFileDialog::ExistingFiles);
    m_file_dialog->setViewMode(QFileDialog::List);
    m_file_dialog->show();
}

/**
 * @brief MainWindow::slot_saveFile
 */
void MainWindow::slot_saveFile()
{
}

/**
 * @brief MainWindow::slot_closeJob
 */
void MainWindow::slot_closeJob(QString job_name, QString file_name)
{
    //when filewidget close file ,all model delete file
    close_checklist_job(job_name);
    delete_checklist_job(job_name);
}

void MainWindow::slot_openREV()
{
    if (m_dir_dialog == NULL)
    {
        m_dir_dialog = new QFileDialog(this);
        m_dir_dialog->setWindowModality(Qt::ApplicationModal);
        m_dir_dialog->setWindowTitle(tr("Open Job"));
        m_dir_dialog->setDirectory("/home/dfjy/workspace/job");
        m_dir_dialog->setNameFilter(tr("Directories"));
        connect(m_dir_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_open_job(QString)), Qt::UniqueConnection);
        m_dir_dialog->setFileMode(QFileDialog::Directory);
        m_dir_dialog->setViewMode(QFileDialog::List);
    }
    m_dir_dialog->show();
}

void MainWindow::slot_open_gauge()
{
    if (m_gauge_dialog == NULL)
    {
        m_gauge_dialog = new QFileDialog(this);
        m_gauge_dialog->setWindowModality(Qt::ApplicationModal);
        m_gauge_dialog->setWindowTitle(tr("Open Gauge"));
        m_gauge_dialog->setDirectory("/home/dfjy/workspace/job");
        m_gauge_dialog->setNameFilter(tr("Directories"));
        connect(m_gauge_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_read_gauge(QString)), Qt::UniqueConnection);
        m_gauge_dialog->setFileMode(QFileDialog::ExistingFile);
        m_gauge_dialog->setNameFilter(tr("file (*.txt)"));
        m_gauge_dialog->setViewMode(QFileDialog::List);
    }
    m_gauge_dialog->show();
}

void MainWindow::slot_coverage_job()
{
    if (m_overlay_dialog == NULL)
    {
        m_overlay_dialog = new QFileDialog(this);
        m_overlay_dialog->setWindowModality(Qt::ApplicationModal);
        m_overlay_dialog->setWindowTitle(tr("Append job"));
        m_overlay_dialog->setDirectory("/home/dfjy/workspace/job");
        m_overlay_dialog->setNameFilter(tr("Directories"));
        connect(m_overlay_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_open_coverage_job(QString)), Qt::UniqueConnection);
        m_overlay_dialog->setFileMode(QFileDialog::Directory);
        m_overlay_dialog->setViewMode(QFileDialog::List);
    }
    m_overlay_dialog->show();
}

void MainWindow::slot_open_coverage_job(QString dirName)
{
    QString oas_path = dirName + "/DefectFile.oas";
    for (int i = 0; i < m_checklist_file_list.count(); i ++)
    {
        if (oas_path == m_checklist_file_list.at(i))
        {
            if (showWarning(this, "Waring", "GDS/Job already opened. Do you want to \nre-open it?", QMessageBox::StandardButtons(QMessageBox::No | QMessageBox::Ok))
                    == QMessageBox::Ok )
            {
                slot_closeJob(dirName, oas_path);
                break;
            }
            else
            {
                return;
            }
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
            add_file(str, true);
            m_checklist_file_list.append(str);
        }
        if(fileInfo.suffix() == "db")
        {
            m_database_path = dirName + "/" + fileInfo.fileName();
            show_checklist(m_database_path);
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
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->set_center_point((end_point_x + start_point_x) / 2, (end_point_y + start_point_y) / 2, (end_point_x - start_point_x) * 2) ;
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
    for (int i = 0; i < m_checklist_file_list.count(); i ++)
    {
        if (oas_path == m_checklist_file_list.at(i))
        {
            if (showWarning(this, "Waring", "GDS/Job already opened. Do you want to \nre-open it?", QMessageBox::StandardButtons(QMessageBox::No | QMessageBox::Ok))
                    == QMessageBox::Ok )
            {
//                fileWidget->delete_file(oas_path);
                slot_closeJob(dirName, oas_path);
                break;
            }
            else
            {
                return;
            }
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
            add_file(str, false);
            m_checklist_file_list.append(str);
            m_open_job_list.append(dirName);
            update_rts_job_commbox(m_open_job_list);
        }
        if(fileInfo.suffix() == "db")
        {
            m_database_path = dirName + "/" + fileInfo.fileName();
            show_checklist(m_database_path);
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

void MainWindow::show_checklist(QString datapath)
{
    emit signal_append_job(datapath);
    checkListDockWidget->show();
}

void MainWindow::close_checklist_job(QString jobName)
{
    for (int i = 0; i < m_checklist_file_list.count(); i ++)
    {
        if (jobName == m_checklist_file_list.at(i))
        {
            emit signal_close_job(jobName);
        }
    }
}

void MainWindow::delete_checklist_job(QString jobName)
{
    for (int i = 0; i < m_checklist_file_list.count(); i ++)
    {
        if (jobName == m_checklist_file_list.at(i))
        {
            m_checklist_file_list.removeAt(i);
            if (m_open_job_list.count() == m_checklist_file_list.count())
            {
                m_open_job_list.removeAt(i);
                update_rts_job_commbox(m_open_job_list);
            }
            else
            {
                logger_console << "open job Number Error!";
            }
        }
    }
}

bool MainWindow::tab_is_job_or_osa(QString filename)
{
    for (int i = 0; i < m_checklist_file_list.count(); i ++)
    {
        if (filename == m_checklist_file_list.at(i))
        {
            return true;
        }
    }
    return false;
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

void MainWindow::add_file(QString filePath, bool isOverLay)
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

    emit signal_addFile(filePath, isOverLay);
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
    add_file(filePath, false);
}

/**
 * @brief MainWindow::slot_create_canvas
 * @param index
 */
void MainWindow::slot_create_canvas(QModelIndex index)
{
    FileProjectWidget::layout_view_iter it = fileWidget->get_layout_view_iter(index.row());
//    m_current_filename = QString::fromStdString((*it)->file_name());
    QString active_filename = QString::fromStdString((*it)->file_name());

    if (!isCanvasExist(it))
    {
        ScaleFrame* frame = m_paint_tabwidget->creat_canvas();

        std::string prep_dir = m_prep_dir.toStdString();
        (*it)->load_into_frame(frame->getRenderFrame(), prep_dir);

//        QStringList list = m_current_filename.split("/", QString::SkipEmptyParts);

        QStringList list = active_filename.split("/", QString::SkipEmptyParts);

        QString filename = list.back();
        m_paint_tabwidget->append_canvas(filename);
        centerWidget_boundingSignal(m_paint_tabwidget->count() - 1);

        //m_checklist_file_list save the oas path
//        if(tab_is_job_or_osa((m_current_filename)))
        if(tab_is_job_or_osa(active_filename))
        {
//            QString path = m_current_filename.left(m_current_filename.size() - 15) + "/defect.db";

            QString path = active_filename.left(active_filename.size() - 15) + "/defect.db";
            show_checklist(path);
        }
    }
    else
    {
        m_paint_tabwidget->set_active_widget((*it)->single_view());
    }
    showCoordinate();
}

void MainWindow::slot_create_overlay_canvas(QModelIndex index)
{

    if (m_current_tabid == -1)
    {
        showWarning(this, "Warning", "Not open canvas !");
    }

    FileProjectWidget::layout_view_iter it = fileWidget->get_layout_view_iter(index.row());
    QString active_filename = QString::fromStdString((*it)->file_name());

    try
    {
        (*it)->attach(m_paint_tabwidget->get_scaleframe(m_current_tabid)->getRenderFrame(), m_prep_dir.toStdString(), true);
    }
    catch(const render::append_error& e)
    {
        showWarning(this, "Warning", QString::fromStdString(e.what()));
        return ;
    }
    catch(const render::append_exception& e)
    {
        ScaleFrame* scale_frame = m_paint_tabwidget->get_scaleframe(m_current_tabid);
        QString canvas_name = scale_frame->get_file_name();
        QString warning_message = QString("%1 \n The canvas name is: %2").arg(QString::fromStdString(e.what())).arg(canvas_name);
        showWarning(this, "Warning",warning_message);
        return ;
    }

    QString file_name(m_paint_tabwidget->get_scaleframe(m_current_tabid)->get_file_name());
    m_paint_tabwidget->setTabText(m_current_tabid, file_name);

    if (tab_is_job_or_osa(active_filename))
    {
        QString path = active_filename.left(active_filename.size() - 15) + "/defect.db";
        show_checklist(path);
    }

    layerwidget->getLayerData(m_paint_tabwidget->get_scaleframe(m_current_tabid)->getRenderFrame());
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
     QString widget_title = "Job" + QString::number(current_defgroup_index) +"_defGroup";
     if (!tableIdIndex.isValid())
     {
         logger_file("DefGroup index error! ");
         return;
     }
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

void MainWindow::initConfigDir()
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

void MainWindow::initPointer()
{
    m_file_dialog = NULL;
    m_dir_dialog = NULL;
    m_setpos_dialog = NULL;
    m_setwindow_dialog = NULL;
    m_overlay_dialog = NULL;
    m_gauge_dialog = NULL;
    m_select_file_dialog = NULL;
}

void MainWindow::initPrepDir()
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
}

void MainWindow::centerWidget_boundingSignal(int index)
{
    connect(this, SIGNAL(signal_setPenWidth(QString)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_pen_width(QString)));
    connect(this, SIGNAL(signal_setPenColor(const QColor&)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_pen_color(const QColor&)));
    connect(m_paint_tabwidget->get_scaleframe(index), SIGNAL(signal_updateDistance(QString)), this, SLOT(slot_updateDistance(QString)));
    connect(m_paint_tabwidget->get_scaleframe(index), SIGNAL(signal_pos_updated(double, double)), this, SLOT(slot_updateXY(double, double)));
    connect(m_paint_toolbar, SIGNAL(signal_setSnapFlag(Global::SnapFLag)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_snapfalg(Global::SnapFLag)));
    connect(this, SIGNAL(signal_set_background_color(QColor)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_background_color(QColor)));
    connect(m_paint_toolbar, SIGNAL(signal_setPaintStyle(Global::PaintTool)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_painter_style(Global::PaintTool)));
    connect(m_paint_toolbar, SIGNAL(signal_all_clear()), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_clear_all()));
    connect(m_paint_toolbar, SIGNAL(siganl_measure_line_clear()), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_clear_measureline()));
    connect(m_paint_toolbar, SIGNAL(signal_mark_clear()), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_clear_mark_point()));
    emit signal_setPenWidth(penWidthCombox->currentText());
    m_paint_toolbar->updata_toolbar();
}


bool MainWindow::isCanvasExist(FileProjectWidget::layout_view_iter it)
{
    return (*it)->has_single_view();
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
                m_paint_tabwidget->slot_close_tab(i);
            }
            else
            {
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

void MainWindow::slot_rtsrun_finished()
{
    m_imagedata_parising->parsing_file();
}

void MainWindow::slot_rts_image_finished()
{
    QString filepath = QDir::homePath() + "/.pangen_gui/pgui_rts/rts_output.oas";
    add_file(filepath, false);
    m_rtsrecview_dialog->show();
    // m_indicator->stopAnimation();
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
    }

    render::LayoutView* from_lv = * (fileWidget->get_layout_view_iter(index));
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

    if(m_select_file_dialog == NULL)
    {
        m_select_file_dialog = new QDialog(this);
        m_file_label = new QLabel("Select file:", m_select_file_dialog);
        m_file_box = new QComboBox(m_select_file_dialog);
        m_select_file_dialog->setWindowTitle("Append file");

        m_select_file_okbutton = new QPushButton("OK", m_select_file_dialog);
        connect(m_select_file_okbutton, SIGNAL(clicked()), this, SLOT(slot_select_append_file()));
        m_select_file_cancelbutton = new QPushButton("Cancel", m_select_file_dialog);
        connect(m_select_file_cancelbutton, SIGNAL(clicked()), m_select_file_dialog, SLOT(close()));
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



    maxWidth += 30;
    m_file_box->setGeometry(30, 60, maxWidth < 150 ? 150 : maxWidth , 30);
    maxWidth += 50;
    m_select_file_dialog->setGeometry(width() / 2 , height() / 2 , maxWidth > 300 ? maxWidth : 300, 140);
    m_select_file_dialog->setFixedSize(maxWidth > 300 ? maxWidth : 300, 140);
    m_file_label->setGeometry(30, 30, 70, 25);
    m_select_file_okbutton->setGeometry(150, 100, 60, 30);
    m_select_file_cancelbutton->setGeometry(220, 100, 60, 30);
    m_select_file_dialog->show();
}

void MainWindow::slot_select_append_file()
{
    m_select_file_dialog->close();
    int index = m_file_box->currentIndex();
    QVariant var = m_file_box->itemData(index);
    QList<QVariant> list = var.toList();
    QVariant to_frame_data = list.at(0);
    QVariant from_lv_data = list.at(1);
    int to_frame_index = to_frame_data.toInt();
    int from_lv_index = from_lv_data.toInt();

    render::LayoutView* from_lv = *(fileWidget->get_layout_view_iter(from_lv_index));

    render::RenderFrame* frame = m_paint_tabwidget->get_scaleframe(to_frame_index)->getRenderFrame();

    if(frame->index_of_layout_views(from_lv) != -1)
    {
        showWarning(this, "Warning", "The selected view has included the file you want to append.");
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

void MainWindow::slot_close_file(QString db_path)
{
    int index = db_path.lastIndexOf("/");
    if(index == -1)
    {
        return ;
    }
    else
    {
        QString file_name = db_path.left(index);
        file_name += "/DefectFile.oas";
        fileWidget->delete_file(file_name);
    }
}
}

