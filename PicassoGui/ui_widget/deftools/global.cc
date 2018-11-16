#include "global.h"
namespace UI {
QVector <QColor> UiStyle::ItemColorList;
QVector <int> UiStyle::ItemPetternList;
QColor UiStyle::DockTitleBarColor = QColor(149, 194, 231);
QColor UiStyle::ToolBarColor = QColor(80, 183, 247, 50);
QColor UiStyle::ButtonPressColor = QColor(80, 183, 180, 50);
QColor UiStyle::ButtonHoverColor = QColor(80, 183, 220, 50);
QString UiStyle::TitleColor = "background-color: rgb(149, 194, 231);";
QString UiStyle::MenuBarColor = "background-color: rgb(131, 170, 193);";
//149, 194, 231
QString UiStyle::StateBarColor = "background-color: rgba(80, 183, 247, 50);";
QString UiStyle::TabWidgetStyle = "background-color: rgb(230, 230, 230);";
QString UiStyle::ActionStyle = \
                                    "QAction{background:rgb(255, 255, 255);"\
                                    "border: 1px solid rgb(80, 183, 220);}"\
                                    "QAction : selected {" \
                                    "background-color: #2dabf9;"\
                                    "}";\
QString UiStyle::FrameBorderColor = \
                              "QFrame#Frame{ border-style: outset;"\
                              "border: 1px solid rgb(50, 50, 50);"\
                              "border-radius:2px;"\
                              "background:rgb(200, 210 ,220);}";
QString UiStyle::MenuBarStyle = \
                              "QMenuBar{border-style:inset;" \
                              "border: 1px solid rgb(150, 150, 120);"\
                              + UiStyle::MenuBarColor + "}";

QString UiStyle::StatusBarStyle = \
                             "QStatusBar{border-style:inset;" \
                             "border: 2px solid rgb(220, 220, 220);"\
                              + UiStyle::MenuBarColor + "}";

QString UiStyle::MenuStyle = \
                            "QMenu{background:rgb(255, 255, 255);"\
                            "border: 1px solid rgb(80, 183, 220);}"\
                            "QMenu::item {"\
                            "background-color: transparent;"\
                            "padding:8px 32px;"\
                            "margin:0px 8px;"\
                            "border-bottom:1px solid #DBDBDB;}"\
                            "QMenu::item:selected {" \
                            "background: #2dabf9;"\
                            "}";\

#if 1
QString UiStyle::TitleStyle = \
                              "QWidget{background-color: "\
                              "qlineargradient(spread:reflect, x1:0.981085,"\
                              "y1:1, x2:0.982641, y2:0, stop:0 rgba(131, 170, 193, 255),"\
                              "stop:1 rgba(255, 255, 255, 255));}";
#else
QString Global::titleStyle = \
                              "QWidget {background-color: "\
                              "qlineargradient(spread:reflect, x1:0.981085,"\
                              "y1:1, x2:0.982641, y2:0, stop:0 rgba(77, 140, 189, 255),"\
                              "stop:1 rgba(255, 255, 255, 255));}";
#endif
#if 0
QString Global::toolbarStyle = \
                             "QToolBar{ background-color:"\
                             "qlineargradient(spread:reflect, x1:0.961538, y1:1, x2:0.021,"\
                             "y2:0, stop:0 rgba(104, 162, 189, 255), stop:1 rgba(255, 255, 255, 255));}";
#else
QString UiStyle::ToolbarStyle = "QToolBar{background-color:rgb(131, 170, 193);}";

QString UiStyle::MainWindowStyle = "QMainWindow::separator {"\
                                    "width: 4px;}"\
                                    "QMainWindow::separator:hover {"\
                                    "background: rgb(0, 180, 200);}"\

                                    "QTabWidget::tab-bar {"\
                                    "border-color:transparent;"\
                                    "background: rgba(80, 183, 247, 50);}"\

                                    "QDialog {"\
                                    "background: rgba(230, 230, 230);}";

#endif
QString UiStyle::ButtonStyle = \
                               "QPushButton{ border:2px;"\
                               "border-style:outset;"\
                               "border-radius:1px;"\
                               "border-color: rgb(150, 150, 150);"\
                               "background:rgb(172, 172, 172);}"\

                               "QPushButton:hover{"\
                               "color:rgb(0, 0, 0);"\
                               "background: rgb(150, 150, 150);}"\

                               "QPushButton:pressed{"\
                               "border:2px; border-style:inset;"\
                               "border-color:rgb(150, 150, 150);"\
                               "background:rgb(125, 125, 125);}";

QString UiStyle::CommboxStyle =\
                              "QComboBox {"\
                              "border:1px solid  rgb(150, 150, 150);"
                              "padding: 1px; min-width:1em;"
                              "background: rgb(230, 230, 230);}"\

                              "QComboBox:editable{"
                              "background: rgb(230, 230, 230);}"

                              "QComboBox QAbstractItemView {"
                              "border: 1px solid rgb(149, 192, 255);"
                              "selection-background-color: rgb(128, 171, 220);}";

QString  UiStyle::DockWidgetStyle = \
                              "QToolBar {"\
                              "border-color:transparent;"\
                              "background: rgba(80, 183, 247, 50);}"\

                              "QTreeView {"\
                              "border:none;"\
                              "background: white;"\
                              "show-decoration-selected: 1;}"\

                              "QTreeView::item {"\
                              "height: 15px;"\
                              "border: none;"\
                              "color: black;"\
                              "background: transparent;}"\

                              "QTreeView::item:hover {"\
                              "background: rgba(128, 171, 220, 60);}"\

                              "QTreeView::item:selected {"\
                              "background: rgb(128, 171, 220);}"\

                              "QTableView {"\
                              "border:none;}"\

                              "QTableView::item{"\
                              "selection-background-color:rgb(128, 171, 220)}"

                              "QHeaderView {"\
                              "background-color: rgb(200, 200, 200);"\
                              "border-radius:0px;"\
                              "min-height:5px;"\
                              "}"\

                              "QHeaderView::section {"\
                              "color: black;"\
                              "background-color: rgb(200, 200, 200);"\
                              "}"\

                              "QDockWidget {"\
                              "border: 3px solid rgb(128, 171, 241);"\
                              "border-style:groove;}"\

                              "QDockWidget::title {background-color: rgb(149, 194, 231); }";
}
