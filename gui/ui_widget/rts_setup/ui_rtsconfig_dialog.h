#ifndef RTSCONFIGWIDGET_H
#define RTSCONFIGWIDGET_H

#include <QDialog>
#include <QTableWidget>
#include <QLayout>

#include "../deftools/defcontrols.h"
#include "../deftools/datastruct.h"
#include "../qt_logger/pgui_log_global.h"
#include "../deftools/cmessagebox.h"
#include "ui_rtsconfig_widget.h"

namespace ui {
class RtsTabStyle : public QProxyStyle
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
            s.rheight() = 22;
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
                    painter->setPen(QColor(50, 50, 50));
                    painter->setBrush(QBrush(QColor(130, 130, 130)));
                    painter->drawRect(allRect.adjusted(1, 1, -1, -1));
                    painter->restore();
                }
                else
                {
                    painter->save();
                    painter->setPen(QColor(80, 80, 80));
                    painter->setBrush(QBrush(QColor(180, 180, 180)));
                    painter->drawRect(allRect.adjusted(3, 3, -3, -3));
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

class RtsConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RtsConfigDialog(QWidget *parent = 0);

    ~RtsConfigDialog();

    void initTopButton();

    void initTabWidget();

    void initBottomButton();

    void initLayout();

    void initConnecttion();

    void initButtonConfig();

    void set_layername_list(const QStringList &);

    void set_canvas_pos(const double&, const double&, const double&, const double&);

    void start();

    void update_job_commbox(QStringList);
    
public slots:
    void slotAddRts();

    void slotDeleteRts();

    void slotCloneRts();

    void slot_ok_button();

    void slot_cancel_button();

    void slot_apply_button();

    void slot_image_parse_finished(int);

    void slot_process_error(const QString &);
signals:
    void signal_get_current_canvaspos();

    void signal_rts_finished(QVector <RtsReviewInfo>);

    void signal_rtsprocess_error(const QString&);

private:
    QPushButton *m_add_button;
    QPushButton *m_clone_button;
    QPushButton *m_delete_button;

    RtsTabWidget *m_rts_tab;
    QVBoxLayout *m_tab_vlayout;

    QHBoxLayout *Hlayout;
    QVBoxLayout *Vlayout;

    QPushButton *m_cancel_button;
    QPushButton *m_ok_button;
    QPushButton *m_apply_button;

    QStringList m_layerdata_list;

    int m_parse_finished_number;
};
}
#endif // RTSCONFIGWIDGET_H
