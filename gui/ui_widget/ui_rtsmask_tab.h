#ifndef UI_RTSMASK_TAB_H
#define UI_RTSMASK_TAB_H
#include <QTabWidget>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>
#include <QProxyStyle>
#include <QBrush>
#include <QRect>
#include <QPainter>
#include <QHeaderView>
#include <QVector>

#include "model/ui_rtsmask_model.h"
#include "delegate/ui_rtsmask_delegate.h"

namespace ui {

class CustomTabStyle : public QProxyStyle
{
public:
    /* sizeFromContents
     *  用于设置Tab标签大小
     * 1.获取原标签大小
     * 2.宽高切换
     * 3.强制宽高
     * 4.return
     * */
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);

        if (type == QStyle::CT_TabBarTab) {
            s.transpose();
            s.rwidth() = 80; // 设置每个tabBar中item的大小
            s.rheight() = 33;
        }
        return s;
    }
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
            {
                QRect allRect = tab->rect;
                if (tab->state & QStyle::State_Selected) {
                    painter->save();
                    painter->setPen(0x89cfff);
                    painter->setBrush(QBrush(0x89cfff));
                    painter->drawRect(allRect.adjusted(6, 6, -6, -6));
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected) {
                    painter->setPen(0xf8fcff);
                }

                else {
                    painter->setPen(0x5d5d5d);
                }
                painter->drawText(allRect, tab->text, option);
                return;
            }
        }
        if (element == CE_TabBarTab) {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};

class MaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaskWidget(QWidget * parent = 0, const QStringList & list = QStringList());
    ~MaskWidget();

    void init_ui();
    void init_tabwidget();
    void init_connection();

    void set_layername_list(const QStringList &);

public slots:
    void slot_add_row();
    void slot_delete_row();
    void slot_clone_row();

private:
    QPushButton *m_mask_add_button;
    QPushButton *m_mask_clone_button;
    QPushButton *m_mask_delete_button;
    QTableView *m_layer_table;
    QLabel *m_boolean_label;
    QLineEdit *m_boolean_edit;
    RtsMaskModel * m_mask_model;
    QStringList m_layername_list;
};

class RtsMaskTab : public QTabWidget
{
    Q_OBJECT
public:
    explicit RtsMaskTab(QWidget *parent = 0);
    void init_tab(const QStringList &);
    void delete_all_tab();
    void set_layername_list(const QStringList &);
    ~RtsMaskTab();

private:
    MaskWidget * m_mask_widget;
    QVector <MaskWidget *> m_maskwidget_vector;
    QStringList m_layername_list;
};

}
#endif // UI_RTSMASKTAB_H
