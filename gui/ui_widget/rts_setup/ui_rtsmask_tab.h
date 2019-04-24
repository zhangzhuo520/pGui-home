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
#include <QDebug>

#include "../model/ui_rtsmask_model.h"
#include "../delegate/ui_rtsmask_delegate.h"
#include "../deftools/datastruct.h"
#include "../qt_logger/pgui_log_global.h"
#include "../deftools/defcontrols.h"

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
            s.rheight() = 25;
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
                    painter->setPen(QColor(120, 120, 120));
                    painter->setBrush(QBrush(QColor(150, 150, 150)));
                    painter->drawRect(allRect.adjusted(-6, 1, -6, -1));
                    painter->restore();
                }
                else
                {
                    painter->save();
                    painter->setPen(QColor(150, 150, 150));
                    painter->setBrush(QBrush(QColor(180, 180, 180)));
                    painter->drawRect(allRect.adjusted(-8, 3, -8, -3));
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected) {
                    painter->setPen(QColor(20, 20, 20));
                }

                else {
                    painter->setPen(QColor(50, 50, 50));
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
    void set_table_data(QStringList);
    void set_boolean(QString);

    const QStringList& get_alisa_list();
    const QStringList& get_layerdata_list();
    QString get_boolean()const;

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

class RtsMaskTab : public RtsTabWidget
{
    Q_OBJECT
public:
    explicit RtsMaskTab(QWidget *parent = 0);
    ~RtsMaskTab();
    RtsMaskTab & operator=(const RtsMaskTab &);
    void init_tab(const QStringList &);
    void delete_all_tab();
    void set_layername_list(const QStringList &);
    QStringList get_alisa_list(int);
    QStringList get_layerdata_list(int);
    QString get_boolean(const int&)const;
    const QStringList& get_layername_list()const;

private:
    MaskWidget * m_mask_widget;
    QVector <MaskWidget *> m_maskwidget_vector;
    QStringList m_layername_list;
};

}
#endif // UI_RTSMASKTAB_H
