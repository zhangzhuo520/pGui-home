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
#include <QLineEdit>
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
#include <QProxyStyle>
#include <QFrame>
#include "global.h"
#include "framelesshelper.h"

class PushButton :public QPushButton
{
    Q_OBJECT
public:
  explicit PushButton(QWidget *parent = 0)
    {
        Q_UNUSED(parent);
        setStyleSheet("QPushButton {"\
                            "background-color: rgba(0, 0, 0, 0);}"\
                      "QPushButton::hover{"\
                                 "background-color: rgba(0, 0, 100, 50);}");\

    }
  explicit PushButton(const QString &text, QWidget *parent=0)
    {
        Q_UNUSED(parent);
        setStyleSheet("QPushButton{"\
                           "background-color: rgba(0, 0, 0, 0);}"\
                      "QPushButton::hover{"\
                           "background-color: rgba(0, 0, 100, 50);}");\
        setText(text);
    }
  ~PushButton(){}
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
protected:
    void paintEvent(QPaintEvent *event);

private:
    QString name;
    PushButton *floatButton;
    PushButton *closeButton;
    QDockWidget *dw;

};

class DockWidget:public QDockWidget
{
    Q_OBJECT
public:
    explicit DockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);

    ~DockWidget();

private:
    void allow(Qt::DockWidgetArea area, bool allow);

    void place(Qt::DockWidgetArea area, bool place);

    DockTitleBar * TitleBar;

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
protected:
    void mouseMoveEvent(QMouseEvent *e)
    {
        setCursor(Qt::ArrowCursor);
        QWidget::mouseMoveEvent(e);
    }

private:
};

class Commbox :public QComboBox
{
    Q_OBJECT
public:
    explicit Commbox(QWidget *parent = 0);
    ~Commbox();
};

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

#endif // DEFCONTROLS_H
