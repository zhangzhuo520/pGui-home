#ifndef CURVE_WIDGET_H
#define CURVE_WIDGET_H

#include <QWidget>
#include <QLayout>
#include <QVector>
#include <QPaintEvent>

#include "qcustomplot.h"
namespace ui {

class SingleWaveWidget : public QCustomPlot
{
    Q_OBJECT
public:
    explicit SingleWaveWidget(QWidget *parent = 0);

    void get_curve_data();

    void paint_curve();
    
signals:
    
public slots:
    void slot_legen_change(QCPLegend::SelectableParts);

    void slot_mousemove_oncurve(QMouseEvent *);

protected:

private:
    void init_connection();

private:

    QCPCurve *m_ai_curve;
    QCPCurve *m_ri_curve;
    QCPCurve *m_ei_curve;

    QCPCurve *m_ai_ils_curve;

    QVector <double> m_ai_x_vector;
    QVector <double> m_ri_x_vector;
    QVector <double> m_ei_x_vector;

    QVector <double> m_ai_y_vector;
    QVector <double> m_ri_y_vector;
    QVector <double> m_ei_y_vector;
};

class CurveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CurveWidget(QWidget *parent = 0);

    void init_plot();

signals:

public slots:

private:
    SingleWaveWidget *m_image_intensity_plot;
    SingleWaveWidget *m_ils_plot;

    QVBoxLayout *m_vbox_layout;

};

}
#endif // CURVE_WIDGET_H
