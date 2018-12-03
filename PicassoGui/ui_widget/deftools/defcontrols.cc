#include "defcontrols.h"
#include <QDebug>
namespace ui {
/**
 * @brief DockWidget::DockWidget
 * @param title
 * @param parent
 * @param flags
 */
DockWidget::DockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    :QDockWidget(parent, flags)
{
    setObjectName(title);
    setWindowTitle(objectName());
    setMinimumHeight(0);
    TitleBar = new DockTitleBar(this, title);
    setTitleBarWidget(TitleBar);
}

/**
 * @brief DockWidget::~DockWidget
 */
DockWidget::~DockWidget()
{
}

/**
 * @brief DockWidget::allow
 * @param area
 * @param allow
 */
void DockWidget::allow(Qt::DockWidgetArea area, bool allow)
{
    Qt::DockWidgetAreas areas = allowedAreas();
    areas = allow ? areas | area : areas & ~area;
    setAllowedAreas(areas);
}

/**
 * @brief DockWidget::place
 * @param area
 * @param place
 */
void DockWidget::place(Qt::DockWidgetArea area, bool place)
{
    if (!place) return;

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    mainWindow->addDockWidget(area, this);
}

/*
 * @brief TabWidget::TabWidget
 * @param parent
 */
TabWidget::TabWidget(QWidget *parent)
{
    Q_UNUSED(parent);
    sizePolicy().setVerticalPolicy(QSizePolicy::Ignored);
    setTabsClosable(true);
    setMouseTracking(true);
}

/**
 * @brief TabWidget::~TabWidget
 */
TabWidget::~TabWidget()
{
}

/**
 * @brief Commbox::Commbox
 * @param parent
 */
Commbox::Commbox(QWidget *parent)
{
    Q_UNUSED(parent);
}

/**
 * @brief Commbox::~Commbox
 */
Commbox::~Commbox()
{
}


DockTitleBar::DockTitleBar(QWidget *parent, QString text)
    : QWidget(parent), titleName(text)
{
    setObjectName(titleName);
    QHBoxLayout *HLayout = new QHBoxLayout(this);
    dw = qobject_cast<QDockWidget*>(parent);
    HLayout->setSpacing(0);
    HLayout->setContentsMargins(0, 0, 0, 0);
    floatButton = new PushButton(this);
    closeButton = new PushButton(this);
    minButton = new PushButton(this);
    floatButton->setIcon(QIcon(":/dfjy/images/float.png"));
    closeButton->setIcon(QIcon(":/dfjy/images/dockclose.png"));
    minButton->setIcon(QIcon(":/dfjy/images/up-trangle.png"));
    HLayout->addSpacerItem(new QSpacerItem(300, 15, QSizePolicy::Ignored,QSizePolicy::Preferred));
    HLayout->addWidget(minButton);
    HLayout->addWidget(floatButton);
    HLayout->addWidget(closeButton);
    HLayout->setSpacing(0);
    HLayout->setContentsMargins(0, 0, 0, 2);
    HLayout->setStretch(0, 30);
    HLayout->setStretch(1, 1);
    HLayout->setStretch(2, 1);
    HLayout->setStretch(3, 1);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(slot_close()));
    connect(floatButton, SIGNAL(clicked()), this, SLOT(slot_float()));
    connect(minButton, SIGNAL(clicked()), this, SLOT(slot_min()));
}

QSize DockTitleBar::minimumSizeHint() const
{
    return QSize(18, 18);
}

void DockTitleBar::slot_close()
{
    dw->close();
}

void DockTitleBar::slot_float()
{
    if (dw->isFloating())
    {
        dw->setFloating(false);
    }
    else
    {
        dw->setFloating(true);
    }
}

void DockTitleBar::slot_min()
{

    if(dw->height() > 20)
    {
//        DockWidgetHight = dw->height();
//        dw->setFixedHeight(18);
////        dw->setMinimumHeight(0);
////        dw->setMaximumHeight(600);
//        minButton->setIcon(QIcon(":/dfjy/images/down-trangle.png"));
    }
    else
    {
//        dw->setFixedHeight(DockWidgetHight);
//        dw->height()
////        dw->setMinimumHeight(0);
////        dw->setMaximumHeight(600);
//        minButton->setIcon(QIcon(":/dfjy/images/up-trangle.png"));
    }
    dw->update();
}

void DockTitleBar::paintEvent(QPaintEvent*e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    QRect rect = this->rect();

    painter.fillRect(rect.left(), rect.top(), rect.width(), rect.height(), UiStyle::DockTitleBarColor);
    painter.drawText(1, 13, titleName);

    painter.setPen(QColor(130, 130, 130));
    painter.drawLine(QPoint(0, rect.height() - 1), QPoint(dw->width(), rect.height() - 1));
}

QSize FlexMenu::FixSize()
{
    return size();
}

void FlexMenu::setFixSize(QSize size)
{
    setFixedSize(size);
}

void FlexWidget::addWidget(QWidget *widget)
{
    Vlayout->addWidget(widget);
}

}
