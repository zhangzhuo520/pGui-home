#ifndef UI_RTSCURVE_H
#define UI_RTSCURVE_H

#include <QDialog>

#include "ui_curve_widget.h"

class QLabel;
class QStatusBar;
class QToolBar;
namespace ui{
class CurveWidget;
class LineData;
class RtsCurve : public QDialog
{
    Q_OBJECT
public:
    explicit RtsCurve(QDialog *parent = 0);
    void draw_curve(const QString &path, const LineData &);
    void culate_cutlint_curve();

signals:
    
public slots:
    void slot_left_action();
    void slot_right_action();
    void slot_home_action();
    void slot_find_action();

private:
    void init_ui();
    void init_toolbar();
    void init_statusbar();

private:
    QToolBar *m_toolbar_widget;
    CurveWidget *m_curve_widget;
    QStatusBar *m_statusbar_widget;
    QLabel *m_coordinate_label;
    QLabel *m_aicd_label;
    QLabel *m_aicd_value;
    QLabel *m_ricd_label;
    QLabel *m_ricd_value;
    QLabel *m_eicd_label;
    QLabel *m_eicd_value;
};
}

#endif // UI_RTSCURVE_H
