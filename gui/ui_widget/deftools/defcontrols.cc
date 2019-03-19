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

    if (title.right(9) == "_category" || title.right(8) == "_defects")
    {
        setAttribute(Qt::WA_DeleteOnClose, true);
    }
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
RtsTabWidget::RtsTabWidget(QWidget *parent)
{
    setParent(parent);
    setStyleSheet(
                "QTabWidget{"\
                "border: 2px solid rgb(80, 183, 220);"\
                "}"

                "QTabWidget::pane{"
                "border-top: 2px solid #C2C7CB;"
                "}"

                "QTabWidget::tab-bar{"
                "left: 5px;"
                "}"

                "QTabWidget QStackedWidget{"
                "background: rgb(220, 220, 220);"
                "}"

                /**********QTabBar**********/
                "QTabBar::tab{"
                "background: rgb(200, 200, 200);"
                "padding: 2px;"
                "}"

                "QTabBar::tab:selected, QTabBar::tab:hover{"
                "background: rgb(150, 150, 150);"
                "}"

                "QTabBar::tab:!selected{"
                "margin-top: 2px;"
                "}"

                /**********QTableView**********/
                "QTableView{"
                "border:none;"
                "background: white;"
                "show-decoration-selected: 1;"
                "}"

                "QTableView::item:selected{"
                "background: rgb(128, 171, 220);"
                "max-height:20px;"
                "min-height:20px;"
                "}");
}

/**
 * @brief TabWidget::~TabWidget
 */
RtsTabWidget::~RtsTabWidget()
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

PageJumpEdit::PageJumpEdit(QWidget *parent)
{
    setParent(parent);
    m_jump_button = new SearchButton("Go", this);
    m_jump_button->setFixedSize(20, 19);
   // m_jump_button->setFixedWidth(20);
    setTextMargins(0, 0, m_jump_button->width(), 0);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);
    hlayout->addWidget(m_jump_button, 0, Qt::AlignRight);
    setLayout(hlayout);

    connect(m_jump_button, SIGNAL(clicked()), this, SLOT(slot_jump_action()));
}

void PageJumpEdit::slot_jump_action()
{
    emit signal_jump(text());
}

SettingDialog::SettingDialog(QWidget *parent, const QString& label_text, const QString& unit_text, const QString& edit_text):
    QDialog(parent)
{
    m_describe_label = new QLabel(label_text, this);
    m_describe_label->setGeometry(30, 30, 65, 25);
    m_input_edit = new QLineEdit(this);
//    m_input_edit->setToolTip("Enter a position as (x, y) in unit um");
    m_input_edit-> setGeometry(100, 30, 150, 25);
    m_input_edit->setText(edit_text);
    m_unit_label = new QLabel(unit_text, this);
    m_unit_label->setGeometry(255, 30, 30, 25);

    m_ok_button = new QPushButton("OK", this);
    m_ok_button->setGeometry(150, 90, 60, 30);
    connect(m_ok_button, SIGNAL(clicked()), this, SLOT(slot_ok_button()));

    m_cancel_button = new QPushButton("Cancel", this);
    m_cancel_button->setGeometry(220, 90, 60, 30);
    connect(m_cancel_button, SIGNAL(clicked()), this, SLOT(slot_cancel_button()));
}

SettingDialog::~SettingDialog()
{
}

QString SettingDialog::get_input_data()
{
    return  m_input_edit->text();
}

bool SettingDialog::get_button_flag()
{
    return m_button_flag;
}

void SettingDialog::slot_cancel_button()
{
    m_button_flag = false;
    done(-1);
}

void SettingDialog::slot_ok_button()
{
    m_button_flag = true;
    done(1);
}

TableView::TableView(QWidget *parent)
{
    setParent(parent);
    setMouseTracking(true);
}

TableView::~TableView()
{
}

void TableView::set_active_item(const QModelIndex& index)
{
    emit clicked(index);
}

void TableView::double_click_item(const QModelIndex & index)
{
    emit doubleClicked(index);
}


TreeView::TreeView(QWidget *parent)
{
    setParent(parent);
}

TreeView::~TreeView()
{
}

void TreeView::set_active_item(const QModelIndex &index)
{
    emit clicked(index);
}

void SearchButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e);
    setCursor(Qt::PointingHandCursor);

}

//SearchButton::~SearchButton()
//{
//}

QProgressIndicator::QProgressIndicator(QWidget* parent)
    : QWidget(parent),
      m_angle(0),
      m_timerId(-1),
      m_delay(40),
      m_displayedWhenStopped(false),
      m_color(Qt::black)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

bool QProgressIndicator::isAnimated () const
{
    return (m_timerId != -1);
}

void QProgressIndicator::setDisplayedWhenStopped(bool state)
{
    m_displayedWhenStopped = state;

    update();
}

bool QProgressIndicator::isDisplayedWhenStopped() const
{
    return m_displayedWhenStopped;
}

void QProgressIndicator::startAnimation()
{
    m_angle = 0;

    if (m_timerId == -1)
        m_timerId = startTimer(m_delay);
}

void QProgressIndicator::stopAnimation()
{
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_timerId = -1;

    update();
}

void QProgressIndicator::setAnimationDelay(int delay)
{
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_delay = delay;

    if (m_timerId != -1)
        m_timerId = startTimer(m_delay);
}

void QProgressIndicator::setColor(const QColor & color)
{
    m_color = color;

    update();
}

QSize QProgressIndicator::sizeHint() const
{
    return QSize(20,20);
}

int QProgressIndicator::heightForWidth(int w) const
{
    return w;
}

void QProgressIndicator::timerEvent(QTimerEvent * /*event*/)
{
    m_angle = (m_angle+30)%360;

    update();
}

void QProgressIndicator::paintEvent(QPaintEvent * /*event*/)
{
    if (!m_displayedWhenStopped && !isAnimated())
        return;

    int width = qMin(this->width(), this->height());

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int outerRadius = (width-1)*0.5;
    int innerRadius = (width-1)*0.5*0.38;

    int capsuleHeight = outerRadius - innerRadius;
    int capsuleWidth  = (width > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
    int capsuleRadius = capsuleWidth/2;

    for (int i=0; i<12; i++)
    {
        QColor color = m_color;
        color.setAlphaF(1.0f - (i/12.0f));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().center());
        p.rotate(m_angle - i*30.0f);
        p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
        p.restore();
    }
}

HistoryLineEdit::HistoryLineEdit(QWidget *parent)
{
    setParent(parent);
    setEditable(true);
    m_input_edit = new QLineEdit(this);
    setLineEdit(m_input_edit);
    setStyleSheet(
                  "QComboBox{"\
                       "border:1px solid  rgb(150, 150, 150);"\
                       "padding: 1px; min-width:1em;"\
                       "background: rgb(255, 255, 255);"\
                  "}"\

                  "QComboBox:editable{"\
                       "background: rgb(255, 255, 255);"\
                  "}"

                  "QComboBox QAbstractItemView{"\
                       "border: 1px solid rgb(149, 192, 255);"\
                       "selection-background-color: rgb(128, 171, 220);"\
                  "}");
}

void HistoryLineEdit::add_history(const QString& s)
{
    if (this->count() > 7)
    {
        this->removeItem(0);
    }
    this->addItem(s);
}

void HistoryLineEdit::setText(const QString &s)
{
    m_input_edit->setText(s);
}

QString HistoryLineEdit::text()
{
    return m_input_edit->text();
}
}

