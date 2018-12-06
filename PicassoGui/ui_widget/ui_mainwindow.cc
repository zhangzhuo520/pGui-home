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
    action = file_menu->addAction(tr("Save"));
    connect(action, SIGNAL(triggered()), this, SLOT(slot_saveFile()));
    action = file_menu->addAction(tr("Close"));
    connect(action, SIGNAL(triggered()), this, SLOT(slot_closeFile()));
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
    QMenu *addon_menu = menuBar()->addMenu(tr("Addon"));
    actionNameList.clear();
    actionNameList << "Defect Review" << "RTS Setup" << "Run RTS"
                   << "Gauge Checker" << "SEM Image Handler"
                   << "Chip Placement Editor";
    for(int i = 0; i < actionNameList.count(); i ++)
    {
        QAction *action = new QAction(actionNameList.at(i), addon_menu);
        action->setObjectName(actionNameList.at(i));
        addon_menu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(slot_AddonActions()));
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
    workspaceDockWidget = new DockWidget(sets[2].name, this, Qt::WindowFlags(sets[2].flags));
    addDockWidget(sets[2].area, workspaceDockWidget);
    checkListDockWidget = new DockWidget(sets[3].name, this, Qt::WindowFlags(sets[3].flags));
    addDockWidget(sets[3].area, checkListDockWidget);
    logDockWidget = new DockWidget(sets[4].name, this, Qt::WindowFlags(sets[4].flags));
    addDockWidget(sets[4].area, logDockWidget);
    broserDockWidget = new DockWidget(sets[5].name, this, Qt::WindowFlags(sets[5].flags));
    addDockWidget(sets[5].area, broserDockWidget);

    tabifyDockWidget(fileDockWidget,layerDockWidget);
    tabifyDockWidget(fileDockWidget,workspaceDockWidget);
    fileDockWidget->raise();
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
    QAction *fileOpen, *fileClose, *fileSave;

    fileOpen = new QAction(QIcon(":/dfjy/images/open.png"), "Open" ,this);
    fileSave = new QAction(QIcon(":/dfjy/images/save.png"), "Save" ,this);
    fileClose = new QAction(QIcon(":/dfjy/images/delete.png"), "Close" ,this);

    FileToolBar->addAction(fileOpen);
    FileToolBar->addAction(fileSave);
    FileToolBar->addAction(fileClose);
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

    QAction *forwardAction = new QAction(QIcon(":/dfjy/images/right.png"),"forward", this);
    QAction *retreatAction = new QAction(QIcon(":/dfjy/images/left.png"),"retreat", this);
    QAction *zoomInAction = new QAction(QIcon(":/dfjy/images/zoomIn.png"),"zoomIn", this);
    QAction *zoomOutAction = new QAction(QIcon(":/dfjy/images/zoomout.png"),"zoomout", this);
    QAction *refreshAction = new QAction(QIcon(":/dfjy/images/refresh.png"),"refresh", this);
    QAction *fullAction = new QAction(QIcon(":/dfjy/images/fullScren.png"),"full", this);

    screenContrlBar->addAction(forwardAction);
    screenContrlBar->addAction(retreatAction);
    screenContrlBar->addAction(zoomInAction);
    screenContrlBar->addAction(zoomOutAction);
    screenContrlBar->addAction(refreshAction);
    screenContrlBar->addAction(fullAction);

    QToolBar *RevToolBar = new QToolBar(this);
    RevToolBar->setWindowTitle(tr("REV Actions"));
    addToolBar(RevToolBar);
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(slot_refreshAction()));

    QAction *revAction = new QAction(QIcon(":/dfjy/images/REV.png"),"REV", this);
    QAction *setAction = new QAction(QIcon(":/dfjy/images/SET.png"),"SET", this);
    QAction *rstAction = new QAction(QIcon(":/dfjy/images/RTS.png"),"RTS", this);
    QAction *gckAction = new QAction(QIcon(":/dfjy/images/GCK.png"),"GCK", this);
    QAction *semAction = new QAction(QIcon(":/dfjy/images/SEM.png"),"SEM", this);
    QAction *cpeAction = new QAction(QIcon(":/dfjy/images/CPE.png"),"CPE", this);

    RevToolBar->addAction(revAction);
    RevToolBar->addAction(setAction);
    RevToolBar->addAction(rstAction);
    RevToolBar->addAction(gckAction);
    RevToolBar->addAction(semAction);
    RevToolBar->addAction(cpeAction);
    connect(revAction, SIGNAL(triggered()), this, SLOT(slot_openREV()));

    QToolBar *PenBar = new QToolBar(this);
    PenBar->setWindowTitle(tr("Pen Actions"));
    addToolBar(PenBar);


    QLabel *penWidthLable = new QLabel("penWidth");

    colorBtn = new PushButton(this);
    QPixmap pixmapColor(20, 20);
    pixmapColor.fill (Qt::white);
    colorBtn->setIcon(QIcon(pixmapColor));

    clearBtn = new PushButton(this);
    clearBtn->setText ("clean");

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
    PenBar->addWidget(clearBtn);

    QToolBar *ScaleToolBar = new QToolBar(this);
    ScaleToolBar->setWindowTitle(tr("Scale Actions"));
    addToolBar(ScaleToolBar);

    QAction *scaleAction = new QAction(QIcon(":/dfjy/images/scale.png"),"scale", this);
    scaleAction->setCheckable(true);
    connect(scaleAction, SIGNAL(toggled(bool)), this, SLOT(slot_showScaleAxis(bool)));
    ScaleToolBar->addAction(scaleAction);
    isShowAxis = false;

    QAction *setPosAction = new QAction(QIcon(":/dfjy/images/found.png"),"setPos", this);
    connect(setPosAction, SIGNAL(triggered()), this, SLOT(slot_setPosAction()));
    ScaleToolBar->addAction(setPosAction);
}

/**
 * @brief MainWindow::initPaintTab
 */
void MainWindow::initPaintTab()
{
    paintTab = new TabWidget(this);
    paintTab->setObjectName("paintTab");
    setCentralWidget(paintTab);
    _isCreatPaintWidget = false;
    QHBoxLayout *hLayout = new QHBoxLayout();
    paintTab->setLayout(hLayout);
    hLayout->setContentsMargins(0, 0, 0, 0);
    connect(paintTab, SIGNAL(tabCloseRequested(int)), this, SLOT(slot_closePaintTab(int)));
    connect(paintTab, SIGNAL(currentChanged(int)), this, SLOT(slot_currentTab_changed(int)));
}

/**
 * @brief MainWindow::initCheckList
 */
void MainWindow::initCheckList()
{
    checklistWidget = new CheckList(this);
    checklistWidget->setMinimumHeight(0);
    checkListDockWidget->resize(100, 300);
    checkListDockWidget->setWidget(checklistWidget);
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
    connect(fileWidget, SIGNAL(close_currentFile(int)), paintTab, SLOT(slot_TabClose(int)));
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
    connect(this, SIGNAL(signal_readDB(QString)), checklistWidget ,SLOT(slot_readDB(QString)));
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

void MainWindow::slot_updataDistance(double length)
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
     isShowAxis = isShow;

     if (paintTab->count() == 0)
     {
         return;
     }

     for (int i = 0; i < m_scaleFrame_vector.count(); i ++)
     {
         showCoordinate();
     }
}

void MainWindow::slot_setPosAction()
{
    setPosWidget = new QDialog(this);
    setPosWidget->setGeometry(width() / 2, height() / 2 , 300, 160);
    setPosX_label = new QLabel("x :", setPosWidget);
    setPosX_label->setGeometry(60, 30, 30, 25);
    setPosX_lineEdit = new QLineEdit(setPosWidget);
    setPosX_lineEdit->setGeometry(100, 30, 150, 25);
    setPosY_label = new QLabel("y :", setPosWidget);
    setPosY_label->setGeometry(60, 80, 30, 25);
    setPosY_lineEdit = new QLineEdit(setPosWidget);
    setPosY_lineEdit->setGeometry(100, 80, 150, 25);
    setPos_foundPushButton = new QPushButton("Ok", setPosWidget);
    setPos_foundPushButton->setGeometry(50, 120, 60, 30);
    connect(setPos_foundPushButton, SIGNAL(clicked()), this, SLOT(slot_setPosButton()));
    setPos_colsePushButton = new QPushButton("Close", setPosWidget);
    setPos_colsePushButton->setGeometry(200, 120, 60, 30);
    connect(setPos_colsePushButton, SIGNAL(clicked()), setPosWidget, SLOT(close()));
    setPosWidget->show();
}

void MainWindow::slot_setPosButton()
{
    if (m_scaleFrame_vector.at(m_current_tabid) != NULL);
    {
        m_scaleFrame_vector.at(m_current_tabid)->set_defect_point(setPosX_lineEdit->text().toDouble(), setPosY_lineEdit->text().toDouble());
    }
}

void MainWindow::slot_refreshAction()
{
    if (paintTab->count() == 0)
    {
        showWarning(this, "Waring", "No open File!");
        return;
    }
    else
    {
//        m_scaleFrame_vector.at(m_current_tabid)->slot_move_point_center();
    }
}

void MainWindow::slot_currentTab_changed(int index)
{
    m_current_tabid = index;
    paintTab->setCurrentIndex(index);
    render::RenderFrame *renderFrame = NULL;
    if (index == -1)
    {
        m_scaleFrame_vector.clear();
        renderFrame = NULL;
    }
    else
    {
        renderFrame = m_scaleFrame_vector.at(m_current_tabid)->getRenderFrame();
    }
    layerwidget->getLayerData(renderFrame, m_current_filename);
}

/**
 * @brief MainWindow::slot_openFile
 */
void MainWindow::slot_openFile()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Open Layout File"));
    fileDialog->setDirectory("/home/dfjy/workspace/job");
    fileDialog->setNameFilter(tr("All layout files(*.oas *.OAS *.oas.gz *.OAS.gz)"));
    connect(fileDialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_addFile(QString)), Qt::UniqueConnection);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::List);
    fileDialog->show();
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
void MainWindow::slot_closeFile()
{
}

void MainWindow::slot_openREV()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Open Layout File"));
    fileDialog->setDirectory("/home/dfjy/workspace/job");
    fileDialog->setNameFilter(tr("Directories"));
    connect(fileDialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_openDB(QString)), Qt::UniqueConnection);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->setViewMode(QFileDialog::List);
    fileDialog->show();
}

/**
 * @brief MainWindow::slot_undo
 */
void MainWindow::slot_undo()
{
}

void MainWindow::slot_drawPoint(const QModelIndex &index)
{
   // QString Stringsize = index.sibling(index.row(), 1).data().toString();
    int point_x = index.sibling(index.row(), 2).data().toDouble();
    int point_y = index.sibling(index.row(), 3).data().toDouble();
    QString Stringsize = index.sibling(index.row(), 1).data().toString();

    for(int i = 0; i < m_scaleFrame_vector.count(); i ++)
    {
        m_scaleFrame_vector.at(i)->drawDefectPoint(point_x, point_y, Stringsize);
    }
}

/**
 * @brief MainWindow::slot_openDB
 * @param dirName
 */
void MainWindow::slot_openDB(QString dirName)
{
    QDir dir(dirName);
    bool isDbFile = false;
    QString DBname = "";
    QFileInfoList flist = dir.entryInfoList();
    foreach(QFileInfo fileInfo, flist)
    {
        if(!fileInfo.isFile())
            continue;
        if(fileInfo.suffix() == "db")
        {
            isDbFile = true;
            DBname = fileInfo.fileName();
        }
        if (fileInfo.suffix() == "oas")
        {
            slot_addFile(dirName + "/" + fileInfo.fileName());
        }
    }
    if (!isDbFile)
    {
        showWarning(this, "" , "Open an invalid dir.");
    }
    else
    {
        DbPath = dirName + "/" + DBname;
        emit signal_readDB(DbPath);
    }
}

/**
 * @brief MainWindow::slot_addFile
 * @param filePath
 */
void MainWindow::slot_addFile(QString filePath)
{
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
    m_current_tabid = index.row();
    if (!isCavseExist(m_current_tabid))
    {
        ScaleFrame *scaleFrame = new ScaleFrame(paintTab);
        std::vector<render::LayoutView>::iterator it = fileWidget->get_layout_view_iter(index.row());
        (*it) = scaleFrame->load_file(m_current_filename, m_prep_dir, false);

        m_scaleFrame_vector.append(scaleFrame);
        paintTab->addTab(scaleFrame, m_current_filename);
        centerWidget_boundingSignal(m_scaleFrame_vector.count() - 1);
    }
    else
    {
        paintTab->setCurrentIndex(m_current_tabid);
    }
    showCoordinate();
}

/**
 * @brief MainWindow::slot_closePaintTab
 * @param index
 */
void MainWindow::slot_closePaintTab(int index)
{
    paintTab->removeTab(index);
}

void MainWindow::slot_updataXY(double x, double y)
{
    currposLable_xNum->setText(QString::number(x));
    currposLable_yNum->setText(QString::number(y));
}

/**
 * @brief MainWindow::slot_showDefGroup
 * @param index
 */
void MainWindow::slot_showDefGroup(QModelIndex index, int jobIndex)
{
     static int oldJobIndex = 0;
     QModelIndex tableIdIndex = index.sibling(index.row(), 1);

    if(!tableIdIndex.data().toString().isEmpty() && oldJobIndex < jobIndex)
    {
        defGroupDockWidget = new DockWidget("Job" + QString::number(jobIndex) +"_defGroup", this, 0);
        addDockWidget(Qt::RightDockWidgetArea, defGroupDockWidget);

        defgroup = new DefGroup(defGroupDockWidget, DbPath, &index, jobIndex);
        defGroupDockWidget->setWidget(defgroup);

        oldJobIndex = jobIndex;

        connect(this, SIGNAL(signal_defgroupUpdata(QModelIndex *)), defgroup, SLOT(slot_DefGroupUpdata(QModelIndex *)));
        connect(defgroup, SIGNAL(signal_showDefects(QModelIndex, int)), this, SLOT(slot_showDefects(QModelIndex, int)));
    }
    else
    {
        emit signal_defgroupUpdata(&index);
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
        defectsDockWidget = new DockWidget("Job" + QString::number(jobIndex) +"_defects", this, 0);
        addDockWidget(Qt::RightDockWidgetArea, defectsDockWidget);
        defectswidget = new DefectsWidget(defectsDockWidget, DbPath, &index, jobIndex);
        defectsDockWidget->setWidget(defectswidget);
        oldJobIndex = jobIndex;
        connect(this, SIGNAL(signal_defectsUpdata(QModelIndex *)), defectswidget, SLOT(slot_defectsUpdata(QModelIndex *)));
        connect(defectswidget->getTableView(), SIGNAL(doubleClicked(const QModelIndex&)), this,  SLOT(slot_drawPoint(const QModelIndex &)));
    }
    else
    {
        emit signal_defectsUpdata(&index);
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

    if (historyFileList.count() > 6)
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
    QSize mainwindow_size = configSet.value("Grometry").toSize();
    resize(mainwindow_size.width(), mainwindow_size.height());
}

void MainWindow::writeSettingConfig()
{
    QSettings configSet(configFile_path + "/configSize.ini", QSettings::IniFormat);
    configSet.setValue("Grometry", size());
}

void MainWindow::addHistoryAction(QString filename)
{
    QAction *action = new QAction(filename, rencentOpen_menu);
    action->setObjectName(filename);
    rencentOpen_menu->addAction(action);
}

void MainWindow::centerWidget_boundingSignal(int index)
{
    connect(this, SIGNAL(signal_setPenWidth(QString)), m_scaleFrame_vector.at(index), SLOT(slot_set_pen_width(QString)));
    connect(this, SIGNAL(signal_setPenColor(const QColor&)), m_scaleFrame_vector.at(index), SLOT(slot_set_pen_color(const QColor&)));
    connect(m_scaleFrame_vector.at(index), SIGNAL(signal_updataDistance(double)), this, SLOT(slot_updataDistance(double)));
    connect(m_scaleFrame_vector.at(index), SIGNAL(signal_pos_updated(double, double)), this, SLOT(slot_updataXY(double, double)));
    connect(this, SIGNAL(signal_setPaintStyle(Global::PaintStyle)), m_scaleFrame_vector.at(index), SLOT(slot_set_painter_style(Global::PaintStyle)));
    emit signal_setPenWidth(penWidthCombox->currentText());
}

bool MainWindow::isCavseExist(int index)
{
    if (index < paintTab->count())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MainWindow::showCoordinate()
{
    if (isShowAxis)
    {
        m_scaleFrame_vector.at(m_current_tabid)->layout()->setContentsMargins(20, 20, 0, 0);
    }
    else
    {
        m_scaleFrame_vector.at(m_current_tabid)->layout()->setContentsMargins(0, 0, 0, 0);
    }
}

}
