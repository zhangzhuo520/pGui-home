#ifndef DEFCONTROLS_H
#define DEFCONTROLS_H

#include <QWidget>
#include <QMenu>
#include <QPushButton>
#include <QPoint>
#include <QTextEdit>
#include <QScrollBar>
#include <QMouseEvent>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>
#include <QTableView>
#include "cmessagebox.h"

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPainter>
#include <QAbstractItemDelegate>
#include <QAbstractItemModel>
#include <QTabWidget>
#include <QSizePolicy>
#include <QComboBox>
#include <QDockWidget>
#include <QMainWindow>
#include <QVariant>
#include <QFrame>
#include <QPointF>
#include <QScrollArea>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QPaintEvent>
#include <QPixmap>
#include <QDebug>
#include <QProxyStyle>
#include <QFrame>
#include <QToolBar>
#include <QEvent>
#include <QPropertyAnimation>
#include <QTreeView>
#include <QEvent>

#include "global.h"
#include "framelesshelper.h"

namespace ui {

class ProxyStyle : public QProxyStyle
{
public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
        QPainter * painter, const QWidget * widget = 0) const
    {
        if (PE_FrameFocusRect == element)
        {
        }
        else
        {
            painter->setPen(Qt::red);
            QProxyStyle::drawPrimitive(element, option,painter, widget);
        }
    }
};

class FlexMenu : public QMenu
{
    Q_OBJECT
    Q_PROPERTY(QSize FixSize READ FixSize WRITE setFixSize)

public:
    explicit FlexMenu(QWidget * parent = 0)
    {
        setParent(parent);
        this->raise();
    }

    QSize FixSize ();

    void setFixSize(QSize);

private:


protected:
    void mousePressEvent(QMouseEvent *e)
    {
        animation = new QPropertyAnimation(this, "size");
        animation->setDuration(50);
        animation->setEasingCurve(QEasingCurve::InQuart);
        animation->setStartValue(QSize(0, 0 ));
        animation->setEndValue(QSize(50, 150));
        mousePressEvent(e);
    }

    QPropertyAnimation *animation;
};

class Toolbar :public QToolBar
{
public:
    explicit Toolbar(QWidget *parent , int width, int heigth):
    m_width(width),
    m_height(heigth)
    {
        Q_UNUSED(parent);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }

    QSize sizeHint() const { return QSize(m_width, 20);}

private:
    int m_width;
    int m_height;
};

class PushButton :public QPushButton
{
    Q_OBJECT
public:
  explicit PushButton(QWidget *parent = 0)
    {
        setParent(parent);
        setStyle(new ProxyStyle());
        setStyleSheet("QPushButton {"\
                      "border:0px;"
                      "background-color: rgba(0, 0, 0, 0);}"\
                      "QPushButton::hover{"\
                      "border:0px;"
                      "background-color: rgba(220, 220, 220);}"
                      "QPushButton::pressed{"\
                      "background-color: rgba(150, 150, 150);}"

                      "QPushButton#markButton,QPushButton#measureLineButton,QPushButton#measureAngleButton,QPushButton#snapButton,QPushButton#measureEraserButton{"\
                      "border:0px;}"
                      "QPushButton#markButton::hover,QPushButton#measureLineButton::hover,QPushButton#measureAngleButton::hover,QPushButton#snapButton::hover,QPushButton#measureEraserButton::hover{"\
                      "border:0px;"
                      "background-color: rgba(180, 180, 180);}"
                      "QPushButton#markButton::checked,QPushButton#measureLineButton::checked,QPushButton#measureAngleButton::checked,QPushButton#snapButton::checked,QPushButton#measureEraserButton::checked{"\
                      "border:0px;"
                      "background-color: rgba(100, 100, 100);}");\

    }
  explicit PushButton(const QString &text, QWidget *parent=0)
    {
        Q_UNUSED(parent);
        setStyleSheet("QPushButton{"\
                      "border:0px;"
                      "background-color: rgba(0, 0, 0, 0);}"\
                      "QPushButton::hover{"\
                      "background-color: rgba(200, 200, 200);}"
                      "QPushButton::pressed{"\
                      "border:0px;"
                      "background-color: rgba(150, 150, 150);}");\
        setText(text);
    }
  ~PushButton(){}
};

class SearchButton :public QPushButton
{
    Q_OBJECT
public:
    explicit SearchButton(QWidget *parent=0)
    {
        Q_UNUSED(parent);
        setStyleSheet("QPushButton{"\
                      "border:0px;"
                      "background-color: rgba(0, 0, 0, 0);}"\
                      "QPushButton::hover{"\
                      "background-color: rgba(200, 200, 200);}"
                      "QPushButton::pressed{"\
                      "border:0px;"
                      "background-color: rgba(150, 150, 150);}");
        setMouseTracking(true);
    }
    explicit SearchButton(const QString &text, QWidget *parent=0)
    {
        Q_UNUSED(parent);
        setStyleSheet("QPushButton{"\
                      "border:0px;"
                      "background-color: rgba(0, 0, 0, 0);}"\
                      "QPushButton::hover{"\
                      "background-color: rgba(200, 200, 200);}"
                      "QPushButton::pressed{"\
                      "border:0px;"
                      "background-color: rgba(150, 150, 150);}");\
        setText(text);
        setMouseTracking(true);
    }
    ~SearchButton();
protected:
    virtual void enterEvent(QEvent *);
};

class MenuButton :public QPushButton
{
    Q_OBJECT
public:
  explicit MenuButton(QWidget *parent = 0)
    {
        Q_UNUSED(parent);
        setStyleSheet("QPushButton {"\
                            "border:0px;"
                            "background-color: rgba(0, 0, 0, 0);}"\
                      "QPushButton::hover{"\
                                 "border:0px;"
                                 "background-color: rgba(0, 0, 100, 50);}");\
    }
  explicit MenuButton(const QString &text, QWidget *parent=0)
    {
        Q_UNUSED(parent);
        setStyleSheet("QPushButton{"\
                           "border:0px;"
                           "background-color: rgba(0, 0, 0, 0);}"\
                      "QPushButton::hover{"\
                      "border:0px;"
                      "background-color: rgba(0, 0, 100, 50);}");\
        setText(text);
    }
signals:
    void signal_leftClick();

protected:
    void mousePressEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton)
        {
              emit signal_leftClick();
        }
        if (e->button() == Qt::RightButton)
        {
              emit click();
        }
        e->accept();
    }

  ~MenuButton(){}
};

class DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    DockTitleBar(QWidget *parent = 0, QString text = "");

    QSize sizeHint() const { return minimumSizeHint(); }
    QSize minimumSizeHint() const;
public slots:
    void slot_close();

    void slot_float();

    void slot_min();
protected:
    void paintEvent(QPaintEvent *event);

private:
    QString titleName;
    PushButton *floatButton;
    PushButton *closeButton;
    PushButton *minButton;
    QDockWidget *dw;
    int DockWidgetHight;

};

class DockWidget:public QDockWidget
{
    Q_OBJECT
public:
    explicit DockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);

    ~DockWidget();

signals:
    void signal_close_database_widget(int);
    void signal_clear_gauge();

protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter *painter = new QPainter();
        QPen pen;
        pen.setColor(UiStyle::ToolBarColor);
        pen.setWidth(3);
        painter->begin(this);
        painter->setPen(pen);
        painter->drawRect(QRect(0, 0, width() - 2, height() - 2));
        painter->end();
    }

    void closeEvent(QCloseEvent *event)
    {
        Q_UNUSED(event);
        if (objectName().right(9) == "_defGroup" || objectName().right(8) == "_defects")
        {
            QString str = objectName().split("_").at(0);
            int index = str.right(str.count() - 3).toInt();
            emit signal_close_database_widget(index);
        }
        if (objectName() == "Gauge")
        {
            emit signal_clear_gauge();
        }
    }

private:
    void allow(Qt::DockWidgetArea area, bool allow);

    void place(Qt::DockWidgetArea area, bool place);

    DockTitleBar * TitleBar;
    QGraphicsDropShadowEffect *effect;
};

/**
 * @brief The TableWidget class
 */
class TabWidget :public QTabWidget
{
      Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);
    ~TabWidget();

signals:
    void signal_mouseMove(const QPoint& p);

public slots:
    void slot_TabClose(int);

private:
    void mouseMoveEvent(QMouseEvent *e)
    {
        setCursor(Qt::ArrowCursor);
        QWidget::mouseMoveEvent(e);
    }


};

class Commbox :public QComboBox
{
    Q_OBJECT
public:
    explicit Commbox(QWidget *parent = 0);
    ~Commbox();
};

class SearchBox :public QFrame
{
public:
    explicit SearchBox(QWidget * parent)
    {
        Q_UNUSED(parent);
        setObjectName("searchedit");
        searchEdit = new QLineEdit(this);
        searchButton = new SearchButton(this);
        QHBoxLayout *Hlayout = new QHBoxLayout(this);
        Hlayout->setContentsMargins(0, 0, 0, 0);
        Hlayout->setSpacing(0);
        Hlayout->addWidget(searchEdit);
        Hlayout->addWidget(searchButton);
        Hlayout->setStretch(0, 8);
        Hlayout->setStretch(0, 1);
        setStyleSheet("QFrame#searchedit{background:rgb(255, 255, 255);"\
                                                "boder:1px solid black;"\
                                                "boder-radius:2px;}");\
    }
private:
    QLineEdit* searchEdit;
    SearchButton *searchButton;
};

class TitleWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int flexhight READ flexhight WRITE setflexhight)
 public:
    explicit TitleWidget(QWidget *parent = 0)
    {
        setParent(parent);
        upButton = new PushButton(this);
        upButton->setGeometry(380, 0, 20, 50);
        upButton->setIcon(QIcon(":/dfjy/images/down-trangle.png"));
        upButton->show();
        connect(upButton, SIGNAL(clicked()),this, SLOT(slot_Amotion()));
    }

    int flexhight()
    {
        return this->height();
    }

    void setflexhight(int heigh)
    {
        setFixedHeight(heigh);
    }
public slots:
    void slot_Amotion()
    {
        animation = new QPropertyAnimation(this, "flexhight");
        animation->setDuration(100);
        animation->setEasingCurve(QEasingCurve::InQuad);
        if (height() > 50)
        {
            animation->setEndValue(50);
        }
        else
        {
            animation->setEndValue(100);
        }
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

protected:
    void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e);
        QPainter paint(this);
        QBrush brush(Qt::red);
        paint.setBrush(brush);
        paint.setPen(Qt::NoPen);
        paint.drawRoundRect(0,  0, width(), height(), 0, 0);
    }
private:
    QPropertyAnimation *animation;

    QPushButton *upButton;
};

class FlexWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlexWidget(QWidget *parent = 0)
    {
        Q_UNUSED(parent);
        setFixedSize(400, 100);
        tWidget = new TitleWidget(this);
        tWidget->setFixedHeight(50);
        tWidget->setFixedWidth(400);
        tWidget->move(0,0);
        Vlayout = new QVBoxLayout(this);
        Vlayout->setSpacing(0);
        Vlayout->setContentsMargins(0, 0, 0, 0);
    }

    void addWidget(QWidget *widget);

private:
    TitleWidget *tWidget;
    QVBoxLayout *Vlayout;
};

class PageJumpEdit :public QLineEdit
{
   Q_OBJECT
public:
    explicit PageJumpEdit(QWidget *parent = 0);

public slots:
    void slot_jump_action();

signals:
    void signal_jump(QString);

private:
    SearchButton * m_jump_button;
};

class SettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingDialog(QWidget *parent, const QString&, const QString&, const QString&);
    ~SettingDialog();
public:
    QString get_input_data();

    bool get_button_flag();

public slots:
    void slot_cancel_button();

    void slot_ok_button();

private:
    QLabel *m_describe_label;
    QLabel *m_unit_label;
    QLineEdit *m_input_edit;

    QPushButton *m_ok_button;
    QPushButton *m_cancel_button;

    bool m_button_flag;
};

class TableView : public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = 0);
    ~TableView();
    void set_active_item(const QModelIndex&);

    void double_click_item(const QModelIndex&);


protected:
    virtual void currentChanged(const QModelIndex &current,
                                const QModelIndex &previous)
    {
        emit clicked(current);
    }
};

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = 0);
    ~TreeView();
    void set_active_item(const QModelIndex&);

protected:
    virtual void currentChanged(const QModelIndex &current,
                                const QModelIndex &previous)
    {
        emit clicked(current);
    }
};

class QProgressIndicator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int delay READ animationDelay WRITE setAnimationDelay)
    Q_PROPERTY(bool displayedWhenStopped READ isDisplayedWhenStopped WRITE setDisplayedWhenStopped)
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    QProgressIndicator(QWidget* parent = 0);

    int animationDelay() const { return m_delay; }

    bool isAnimated () const;

    bool isDisplayedWhenStopped() const;

    const QColor & color() const { return m_color; }

    virtual QSize sizeHint() const;
    int heightForWidth(int w) const;
public slots:
    void startAnimation();

    void stopAnimation();

    void setAnimationDelay(int delay);

    void setDisplayedWhenStopped(bool state);

    void setColor(const QColor & color);
protected:
    virtual void timerEvent(QTimerEvent * event);
    virtual void paintEvent(QPaintEvent * event);
private:
    int m_angle;
    int m_timerId;
    int m_delay;
    bool m_displayedWhenStopped;
    QColor m_color;
};

}
#endif // DEFCONTROLS_H
