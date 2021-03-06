#include "ui_mainwindow.h"
namespace ui {
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setObjectName("MainWindow");

    setWindowTitle("pGui");

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

    initConnection();

    initPointer();

    initPrepDir();

    initStyle();

    readSettingConfig();
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
    QStringList actionNameList;
    action->setShortcuts(QKeySequence::Open);
    connect(action, SIGNAL(triggered()), this, SLOT(slot_openFile()));
//    action = file_menu->addAction(tr("Save"));
    connect(action, SIGNAL(triggered()), this, SLOT(slot_saveFile()));
//    action = file_menu->addAction(tr("Close"));
//    connect(action, SIGNAL(triggered()), this, SLOT(slot_closeFile()));
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
    actionNameList.clear();
    actionNameList << "Defect Review" << "RTS Setup" << "Run RTS"
                   << "Gauge Checker" << "SEM Image Handler"
                   << "Chip Placement Editor";
    for(int i = 0; i < actionNameList.count(); i ++)
    {
//        QAction *action = new QAction(actionNameList.at(i), addon_menu);
//        action->setObjectName(actionNameList.at(i));
//        addon_menu->addAction(action);
//        connect(action, SIGNAL(triggered()), this, SLOT(slot_AddonActions()));
    }
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
        { "ChickList", 0, Qt::BottomDockWidgetArea },
        { "Log", 0, Qt::BottomDockWidgetArea },
        { "Broser", 0, Qt::RightDockWidgetArea }
    };

    fileDockWidget = new DockWidget(sets[0].name, this, Qt::WindowFlags(sets[0].flags));
    addDockWidget(sets[0].area, fileDockWidget);
    layerDockWidget = new DockWidget(sets[1].name, this, Qt::WindowFlags(sets[1].flags));
    addDockWidget(sets[1].area, layerDockWidget);
    //workspaceDockWidget = new DockWidget(sets[2].name, this, Qt::WindowFlags(sets[2].flags));
    //addDockWidget(sets[2].area, workspaceDockWidget);
    checkListDockWidget = new DockWidget(sets[3].name, this, Qt::WindowFlags(sets[3].flags));
    addDockWidget(sets[3].area, checkListDockWidget);
    //logDockWidget = new DockWidget(sets[4].name, this, Qt::WindowFlags(sets[4].flags));
    //addDockWidget(sets[4].area, logDockWidget);
    //broserDockWidget = new DockWidget(sets[5].name, this, Qt::WindowFlags(sets[5].flags));
    //addDockWidget(sets[5].area, broserDockWidget);

    tabifyDockWidget(fileDockWidget,layerDockWidget);
//    tabifyDockWidget(fileDockWidget,workspaceDockWidget);
    fileDockWidget->raise();
    checkListDockWidget->hide();
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

    RevToolBar->addAction(revAction);
    connect(revAction, SIGNAL(triggered()), this, SLOT(slot_openREV()));

    QToolBar *PenBar = new QToolBar(this);
    PenBar->setWindowTitle(tr("Pen Actions"));
    addToolBar(PenBar);


    QLabel *penWidthLable = new QLabel("penWidth");

    colorBtn = new PushButton(this);
    QPixmap pixmapColor(20, 20);
    pixmapColor.fill (Qt::white);
    colorBtn->setIcon(QIcon(pixmapColor));

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
    connect (colorBtn, SIGNAL(clicked()), this, SLOT(ShowColorDialog()), Qt::UniqueConnection);
    PenBar->addWidget(penWidthCombox);
    PenBar->addWidget(colorBtn);

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
}

/**
 * @brief MainWindow::initm_paint_tabwidget
 */
void MainWindow::initPaintTab()
{
    m_current_tabid = 0;
//    m_scrollarea_bar = new QScrollArea(this);
//    setCentralWidget(m_scrollarea_bar);
    m_center_widget = new QWidget(this);
//    m_center_widget->setGeometry(0, 0, centralWidget()->width(), centralWidget()->height());
    m_paint_tabwidget = new TabPaintWidget(this);
    m_paint_toolbar = new PaintToolbar(centralWidget());

    setCentralWidget(m_center_widget);
    m_paint_tabwidget->setObjectName("m_paint_tabwidget");
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    vLayout->addWidget(m_paint_tabwidget);
    vLayout->addWidget(m_paint_toolbar);

    m_center_widget->setLayout(vLayout);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    m_paint_tabwidget->setLayout(hLayout);

    connect(m_paint_tabwidget, SIGNAL(tabCloseRequested(int)), this, SLOT(slot_close_paintwidget(int)));
    connect(m_paint_tabwidget, SIGNAL(currentChanged(int)), this, SLOT(slot_currentTab_changed(int)));
    connect(this, SIGNAL(signal_setPaintStyle(Global::PaintStyle)), m_paint_toolbar, SLOT(slot_setPaintStyle(Global::PaintStyle)));
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
}

/**
 * @brief MainWindow::init_fileProject_fileTab
 */
void MainWindow::init_fileProject_widget()
{
    fileWidget = new FileProjectWidget(this);
    fileWidget->setMinimumHeight(0);
    fileDockWidget->setWidget(fileWidget);
    connect(fileWidget, SIGNAL(signal_DoubleClickItem(QModelIndex)), this, SLOT(slot_creat_canvas(QModelIndex)));
    connect(this, SIGNAL(signal_addFile(QString)), fileWidget, SLOT(slot_addFile(QString)));
    connect(fileWidget, SIGNAL(signal_close_currentFile(QString)), this, SLOT(slot_closeFile(QString)));
}

/**
 * @brief MainWindow::init_fileProject_layerTree
 */
void MainWindow::init_fileProject_layerTree()
{
    layerwidget = new LayerWidget(this);
    layerwidget->setMinimumHeight(0);
    layerDockWidget->setWidget(layerwidget);
}

/**
 * @brief MainWindow::init_fileProject_workSpace
 */
void MainWindow::init_fileProject_workSpace()
{
    broserDockWidget->setFixedHeight(50);
}

/**
 * @brief MainWindow::initConnection
 */
void MainWindow::initConnection()
{
    connect(this, SIGNAL(singal_append_job(QString)), checklistWidget, SLOT(slot_append_job(QString)));
    connect(this, SIGNAL(signal_close_job(QString)), checklistWidget, SLOT(slot_close_job(QString)));
    connect(checklistWidget, SIGNAL(signal_close_job(QString)), m_paint_tabwidget, SLOT(slot_close_tab(QString)));
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

void MainWindow::DefectReview()
{
}

void MainWindow::RTSSetup()
{
    RtsConfigDialog *RTSSetupDialog = new RtsConfigDialog(this);
    RTSSetupDialog->setGeometry(200, 200, 400, 600);
    RTSSetupDialog->show();
}

void MainWindow::RunRTS()
{

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

void MainWindow::ShowColorDialog()
{
    QColor color = QColorDialog::getColor();

    if (color.isValid ())
    {
        QPixmap p(20, 20);
        p.fill (color);
        colorBtn->setIcon (QIcon(p));
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
    emit signal_setPaintStyle(paintstyle);
}

void MainWindow::slot_penAction()
{
    mouseAction->setChecked(false);
    penAction->setChecked(true);
    rulerAction->setChecked(false);
    paintstyle = Global::Mark;
    emit signal_setPaintStyle(paintstyle);
}

void MainWindow::slot_rulerAction()
{
    mouseAction->setChecked(false);
    penAction->setChecked(false);
    rulerAction->setChecked(true);
    paintstyle = Global::Measrue;
    emit signal_setPaintStyle(paintstyle);
}

void MainWindow::slot_updateDistance(double length)
{
    distanceLableNum->setText(QString::number(length));
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
    }

    m_setpos_dialog->setWindowTitle("Set Position");
    m_setpos_dialog->setGeometry(width() / 2 - 150, height() / 2 - 150 , 300, 140);
    m_pos_label = new QLabel("Pos(x, y):", m_setpos_dialog);
    m_pos_label->setGeometry(30, 30, 65, 25);
    m_pos_lineeidt = new QLineEdit(m_setpos_dialog);
    m_pos_lineeidt->setToolTip("Enter a position as (x, y) in unit um");
    m_pos_lineeidt-> setGeometry(100, 30, 150, 25);
    m_pos_lineeidt->setText(",");
    m_pos_lineeidt->setCursorPosition(0);
    m_pos_unit_label = new QLabel("um", m_setpos_dialog);
    m_pos_unit_label->setGeometry(255, 30, 30, 25);

    m_setpos_okbutton = new QPushButton("Ok", m_setpos_dialog);
    m_setpos_okbutton->setGeometry(150, 90, 60, 30);
    connect(m_setpos_okbutton, SIGNAL(clicked()), this, SLOT(slot_setPosButton()));
    m_setpos_cancelbutton = new QPushButton("Cancel", m_setpos_dialog);
    m_setpos_cancelbutton->setGeometry(220, 90, 60, 30);
    connect(m_setpos_cancelbutton, SIGNAL(clicked()), m_setpos_dialog, SLOT(close()));
    m_setpos_dialog->show();
}

void MainWindow::slot_setWindowMaxSizeAction()
{
    if(m_setwindow_dialog == NULL)
    {
        m_setwindow_dialog = new QDialog(this);
    }
    m_setwindow_dialog->setWindowTitle("set WindowMaxSize");
    m_setwindow_dialog->setGeometry(width() / 2 - 150, height() / 2 - 150, 300, 140);
    m_window_label = new QLabel("Size:", m_setwindow_dialog);
    m_window_label->setGeometry(30, 30, 70, 25);
    m_window_lineedit = new QLineEdit(m_setwindow_dialog);
    m_window_lineedit->setGeometry(100, 30, 150, 25);

    if ((m_current_tabid < m_paint_tabwidget->count()) && (m_paint_tabwidget->count() > 0))
    {
        double limit = m_paint_tabwidget->get_scaleframe(m_current_tabid)->get_window_max_size();
        std::ostringstream ss;
        ss << limit;
        m_window_lineedit->setText(QString::fromStdString(ss.str()));
    }
    else
    {
        m_window_lineedit->setText("120.0");
    }

    m_window_lineedit->setCursorPosition(0);
    m_window_unit_label = new QLabel("um", m_setwindow_dialog);
    m_window_unit_label->setGeometry(255, 30, 30, 25);
    m_setwindow_okbutton = new QPushButton("Ok", m_setwindow_dialog);
    m_setwindow_okbutton->setGeometry(150, 90, 60, 30);
    connect(m_setwindow_okbutton, SIGNAL(clicked()), this, SLOT(slot_setWindowMaxSizeButton()));
    m_setwindow_cancelbutton = new QPushButton("Cancel", m_setwindow_dialog);
    m_setwindow_cancelbutton->setGeometry(220, 90, 60, 30);
    connect(m_setwindow_cancelbutton, SIGNAL(clicked()), m_setwindow_dialog, SLOT(close()));
    m_setwindow_dialog->show();
}

void MainWindow::slot_setPosButton()
{
    QStringList list =  m_pos_lineeidt->text().split(',');
    if (list.count() != 2)
    {
        return;
    }
    else
    {
        if ((m_current_tabid < m_paint_tabwidget->count()) && (m_paint_tabwidget->count() > 0))
        {
            m_paint_tabwidget->get_scaleframe(m_current_tabid)->set_defect_point(list.at(0).toDouble(), list.at(1).toDouble());
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
    QString defect = "Job" + QString::number(job_number) +"_defects";
    if (findChild < QWidget * >(defgroup) == 0)
    {
        return;
    }
    DockWidget *defgroup_widget = findChild < DockWidget * >(defgroup);
    DockWidget *defect_widget = findChild < DockWidget * >(defect);
    defgroup_widget->close();
    defect_widget->close();
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
        m_current_filename = QString::fromStdString(renderFrame->get_layout_view(index).file_name());
    }
    layerwidget->getLayerData(renderFrame, m_current_filename);

}

/**
 * @brief MainWindow::slot_closem_paint_tabwidget
 * @param index
 */
void MainWindow::slot_close_paintwidget(int index)
{
    QString filename = m_paint_tabwidget->tabText(index);
    m_paint_tabwidget->slot_close_tab(filename);
    close_checklist_job(filename);
}

//void MainWindow::slot_disable_draw_defects()
//{
//    for(int i = 0; i < m_paint_tabwidget->count(); i ++)
//    {
//        m_paint_tabwidget->get_scaleframe(i)->disableDrawDefects();
//    }
//}

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
 * @brief MainWindow::slot_closeFile
 */
void MainWindow::slot_closeFile(QString filename)
{
    //when filewidget close file ,all model delete file
    m_paint_tabwidget->slot_close_tab(filename);
    close_checklist_job(filename);
    delete_checklist_job(filename);
}

void MainWindow::slot_openREV()
{
    if (m_dir_dialog == NULL)
    {
        m_dir_dialog = new QFileDialog(this);
    }
    m_dir_dialog->setWindowModality(Qt::ApplicationModal);
    m_dir_dialog->setWindowTitle(tr("Open Layout File"));
    m_dir_dialog->setDirectory("/home/dfjy/workspace/job");
    m_dir_dialog->setNameFilter(tr("Directories"));
    connect(m_dir_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_open_job(QString)), Qt::UniqueConnection);
    m_dir_dialog->setFileMode(QFileDialog::Directory);
    m_dir_dialog->setViewMode(QFileDialog::List);
    m_dir_dialog->show();
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

    for(int i = 0; i < m_paint_tabwidget->count(); i ++)
    {
        m_paint_tabwidget->get_scaleframe(i)->drawDefectPoint(point_x, point_y, Stringsize);
    }
}

void MainWindow::slot_open_job(QString dirName)
{
    QString temp_str = dirName + "/DefectFile.oas";
    for (int i = 0; i < m_checklist_file_list.count(); i ++)
    {
        if (temp_str == m_checklist_file_list.at(i))
        {
            if (showWarning(this, "Waring", "GDS/Job already opened. Do you want to \nre-open it?", QMessageBox::StandardButtons(QMessageBox::No | QMessageBox::Ok))
                    == QMessageBox::Ok )
            {
                fileWidget->delete_file(temp_str);
                slot_closeFile(temp_str);
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
            slot_addFile(str);
            m_checklist_file_list.append(str);
        }
        if(fileInfo.suffix() == "db")
        {
            m_database_path = dirName + "/" + fileInfo.fileName();
            show_checklist(m_database_path);
        }
    }
}

void MainWindow::show_checklist(QString datapath)
{
    emit singal_append_job(datapath);
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


/**
 * @brief MainWindow::slot_addFile
 * @param filePath
 */
void MainWindow::slot_addFile(QString filePath)
{
    if (fileWidget->is_file_exist(filePath))
    {
        if (showWarning(this, "Waring", "GDS/Job already opened. Do you want to \nre-open it?", QMessageBox::StandardButtons(QMessageBox::No | QMessageBox::Ok))
                == QMessageBox::Ok )
        {
            fileWidget->delete_file(filePath);
        }
        else
        {
            return;
        }
    }
    emit signal_addFile(filePath);

    saveOpenHistory(filePath);
    addHistoryAction(filePath);
}

/**
 * @brief MainWindow::slot_creat_canvas
 * @param index
 */
void MainWindow::slot_creat_canvas(QModelIndex index)
{
    m_current_filename = index.data().toString();
    if (!isCavseExist(m_current_filename))
    {
        m_paint_tabwidget->creat_canvas();
        std::vector<render::LayoutView>::iterator it = fileWidget->get_layout_view_iter(index.row());
        (*it) = m_paint_tabwidget->load_file(m_current_filename, m_prep_dir, false);
        m_paint_tabwidget->append_canvas(m_current_filename);
        centerWidget_boundingSignal(m_paint_tabwidget->count() - 1);

        if (tab_is_job_or_osa(m_current_filename))
        {
            QString path = m_current_filename.left(m_current_filename.size() - 15) + "/defect.db";
            show_checklist(path);
        }
    }
    else
    {
        m_paint_tabwidget->set_active_widget(m_current_filename);
    }
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
     static int defgroup_count = 0;
     QModelIndex tableIdIndex = index.sibling(index.row(), 9);
     QString widget_title = "Job" + QString::number(current_defgroup_index) +"_defGroup";
     if (!tableIdIndex.isValid())
     {
         return;
     }

    if(!tableIdIndex.data().toString().isEmpty() && defgroup_count < current_defgroup_index)
    {
        DockWidget *defGroupDockWidget = new DockWidget(widget_title, this, 0);
        m_defgroupdockwidget_vector.append(defGroupDockWidget);
        addDockWidget(Qt::RightDockWidgetArea, defGroupDockWidget);

        DefGroup *defgroup = new DefGroup(defGroupDockWidget, m_database_path, &index, current_defgroup_index);
        m_defgroup_vector.append(defgroup);
        defGroupDockWidget->setWidget(m_defgroup_vector.at(m_defgroup_vector.count() - 1));
        defgroup_count = current_defgroup_index;        //defgrout always is most large defgroup index

        connect(m_defgroup_vector.at(m_defgroup_vector.count() - 1), SIGNAL(signal_showDefects(QModelIndex, int)), this, SLOT(slot_showDefects(QModelIndex, int)));
    }
    else
    {
        if (m_defgroupdockwidget_vector.count() < 1 || current_defgroup_index < 1)
        {
            return;
        }
        m_defgroupdockwidget_vector.at(current_defgroup_index - 1)->show();
        m_defgroup_vector.at(current_defgroup_index - 1)->updata_all_data(&index);
    }
}

/**
 * @brief MainWindow::slot_showDefects
 * @param index
 * @param jobIndex
 */
void MainWindow::slot_showDefects(QModelIndex index, int jobIndex)
{
    static int oldJobIndex = 0;
    if(!index.data().toString().isEmpty() && oldJobIndex < jobIndex)
    {
        DockWidget* defectsDockWidget = new DockWidget("Job" + QString::number(jobIndex) +"_defects", this, 0);
        addDockWidget(Qt::RightDockWidgetArea, defectsDockWidget);
        m_defectsdockwidget_vector.append(defectsDockWidget);
        DefectsWidget *defectswidget = new DefectsWidget(defectsDockWidget, m_database_path, &index, jobIndex);
        m_defectswidget_vector.append(defectswidget);
        defectsDockWidget->setWidget(m_defectswidget_vector.at(m_defectswidget_vector.count() - 1));
        oldJobIndex = jobIndex;
        connect(defectswidget->getTableView(), SIGNAL(clicked(const QModelIndex&)), this,  SLOT(slot_drawPoint(const QModelIndex &)));
//        connect(defectswidget, SIGNAL(signal_disable_draw_defects()), this, SLOT(slot_disable_draw_defects()));
    }
    else
    {
        m_defectsdockwidget_vector.at(jobIndex - 1)->show();
        m_defectswidget_vector.at(jobIndex - 1)->update_all_data(&index);
    }
}

void MainWindow::initConfigDir()
{
    configFile_path = QDir::homePath() + "/.pgui_config";
    QDir dir(configFile_path);

    if (!dir.exists())
    {
        if(!dir.mkdir(configFile_path))
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
}

void MainWindow::initPrepDir()
{
    m_prep_dir = QDir::homePath() + "/.PguiPrep";
    QDir dir(m_prep_dir);
    if (!dir.exists())
    {
         if(!dir.mkdir(m_prep_dir))
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
    connect(m_paint_tabwidget->get_scaleframe(index), SIGNAL(signal_updateDistance(double)), this, SLOT(slot_updateDistance(double)));
    connect(m_paint_tabwidget->get_scaleframe(index), SIGNAL(signal_pos_updated(double, double)), this, SLOT(slot_updateXY(double, double)));
    connect(m_paint_toolbar, SIGNAL(signal_setSnapFlag(Global::SnapFLag)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_snapfalg(Global::SnapFLag)));
    connect(m_paint_toolbar, SIGNAL(signal_setPaintStyle(Global::PaintTool)), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_set_painter_style(Global::PaintTool)));
    connect(m_paint_toolbar, SIGNAL(signal_measure_clear()), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_clear_measureline()));
    connect(m_paint_toolbar, SIGNAL(signal_mark_clear()), m_paint_tabwidget->get_scaleframe(index), SLOT(slot_clear_mark_point()));
    emit signal_setPenWidth(penWidthCombox->currentText());
}

bool MainWindow::isCavseExist(QString filename)
{
    for (int i = 0; i < m_paint_tabwidget->count(); i ++)
    {
        if (filename == m_paint_tabwidget->tabText(i))
        {
            return true;
        }
    }
    return false;
}

void MainWindow::showCoordinate()
{
    if (m_axis_show)
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->layout()->setContentsMargins(20, 20, 0, 0);
    }
    else
    {
        m_paint_tabwidget->get_scaleframe(m_current_tabid)->layout()->setContentsMargins(0, 0, 0, 0);
    }
}

}
