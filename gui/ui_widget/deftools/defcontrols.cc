#include "defcontrols.h"
#include <QDebug>

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
    TitleBar = new DockTitleBar(this, title);
    setTitleBarWidget(TitleBar);
    setStyleSheet(UiStyle::DockWidgetStyle);
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
    setStyleSheet(UiStyle::TabWidgetStyle);
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
    setStyleSheet(UiStyle::CommboxStyle);
}

/**
 * @brief Commbox::~Commbox
 */
Commbox::~Commbox()
{
}


DockTitleBar::DockTitleBar(QWidget *parent, QString text)
    : QWidget(parent), name(text)
{
    QHBoxLayout *HLayout = new QHBoxLayout(this);
    dw = qobject_cast<QDockWidget*>(parentWidget());
    HLayout->setSpacing(0);
    HLayout->setContentsMargins(0, 0, 0, 0);
    floatButton = new PushButton(this);
    closeButton = new PushButton(this);
    floatButton->setIcon(QIcon(":/dfjy/images/float.png"));
    closeButton->setIcon(QIcon(":/dfjy/images/dockclose.png"));
    HLayout->addSpacerItem(new QSpacerItem(300, 15, QSizePolicy::Ignored,QSizePolicy::Preferred));
    HLayout->addWidget(floatButton);
    HLayout->addWidget(closeButton);
    HLayout->setSpacing(0);
    HLayout->setContentsMargins(0, 0, 0, 2);
    HLayout->setStretch(0, 30);
    HLayout->setStretch(1, 1);
    HLayout->setStretch(2, 1);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(slot_close()));
    connect(floatButton, SIGNAL(clicked()), this, SLOT(slot_float()));
}

QSize DockTitleBar::minimumSizeHint() const
{
    QDockWidget *dw = qobject_cast<QDockWidget*>(parentWidget());
    Q_ASSERT(dw != 0);
    QSize result(10, 18);
    if (dw->features() & QDockWidget::DockWidgetVerticalTitleBar)
        result.transpose();
    return result;
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

void DockTitleBar::paintEvent(QPaintEvent*e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    QRect rect = this->rect();

    QDockWidget *dw = qobject_cast<QDockWidget*>(parentWidget());
    Q_ASSERT(dw != 0);

    if (dw->features() & QDockWidget::DockWidgetVerticalTitleBar) {
        QSize s = rect.size();
        s.transpose();
        rect.setSize(s);

        painter.translate(rect.left(), rect.top() + rect.width());
        painter.rotate(-90);
        painter.translate(-rect.left(), -rect.top());
    }

    painter.fillRect(rect.left(), rect.top(), rect.width(), rect.height(), UiStyle::DockTitleBarColor);
    painter.drawText(1, 13, name);

    painter.setPen(QColor(130, 130, 130));
    painter.drawLine(QPoint(0, rect.height() - 1), QPoint(dw->width(), rect.height() - 1));
}

