#include "ui_curve_widget.h"
namespace ui {

SingleWaveWidget::SingleWaveWidget(QWidget *parent):
    QCustomPlot(parent)
{
    get_curve_data();

    paint_curve();

    init_connection();
}

void SingleWaveWidget::get_curve_data()
{
    for (int i = 0; i < 100; i ++)
    {
        double data = (double)(rand()/(double)RAND_MAX);
        m_ai_x_vector.append(data);
        data = (double)(rand()/(double)RAND_MAX);
        m_ri_x_vector.append(data);
        data = (double)(rand()/(double)RAND_MAX);
        m_ei_x_vector.append(data);

        data = (double)(rand()/(double)RAND_MAX);
        m_ai_y_vector.append(data);
        data = (double)(rand()/(double)RAND_MAX);
        m_ri_y_vector.append(data);
        data = (double)(rand()/(double)RAND_MAX);
        m_ei_y_vector.append(data);
    }
}

void SingleWaveWidget::paint_curve()
{
    setInteractions(QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes | QCP::iSelectPlottables);
    plotLayout()->clear();

    QCPAxisRect *image_intensity_plot = new QCPAxisRect(this);
    plotLayout()->addElement(0, 0, image_intensity_plot);
    image_intensity_plot->axis(QCPAxis::atBottom)->setLabel(QString("Distance(nm)"));

    plotLayout()->setRowSpacing(0);
    image_intensity_plot->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msAll);
    image_intensity_plot->setMargins(QMargins(0, 0, 0, 0));

    m_ai_curve = new QCPCurve(image_intensity_plot->axis(QCPAxis::atBottom), image_intensity_plot->axis(QCPAxis::atLeft));
    m_ai_curve->setName("AiImage");
    m_ai_curve->setPen(QColor(100, 180, 110));

    m_ri_curve = new QCPCurve(image_intensity_plot->axis(QCPAxis::atBottom), image_intensity_plot->axis(QCPAxis::atLeft));
    m_ri_curve->setName("RiImage");
    m_ri_curve->setPen(QColor(180, 90, 90));

    m_ei_curve = new QCPCurve(image_intensity_plot->axis(QCPAxis::atBottom), image_intensity_plot->axis(QCPAxis::atLeft));
    m_ei_curve->setName("EiImage");
    m_ei_curve->setPen(QColor(220, 40, 40));

    for (int i = 0; i < 100; i ++)
    {
        m_ai_curve->setData(m_ai_x_vector, m_ai_y_vector);
        m_ri_curve->setData(m_ri_x_vector, m_ri_y_vector);
        m_ei_curve->setData(m_ei_x_vector, m_ei_y_vector);
    }

    QCPAxisRect *ils_plot = new QCPAxisRect(this);
    plotLayout()->addElement(1, 0, ils_plot);
    ils_plot->axis(QCPAxis::atBottom)->setLabel(QString("Distance(nm)"));

    plotLayout()->setRowSpacing(0);
    ils_plot->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msAll);
    ils_plot->setMargins(QMargins(0, 0, 0, 0));

    m_ai_ils_curve = new QCPCurve(ils_plot->axis(QCPAxis::atBottom), ils_plot->axis(QCPAxis::atLeft));
    m_ai_ils_curve->setName("AiImage");
    m_ai_ils_curve->setPen(QColor(100, 180, 110));


    for (int i = 0; i < 100; i ++)
    {
        m_ai_ils_curve->setData(m_ai_x_vector, m_ai_y_vector);
    }

    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), ils_plot->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(ils_plot->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this->xAxis, SLOT(setRange(QCPRange)));


    QCPMarginGroup *group = new QCPMarginGroup(this);
    this->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ils_plot->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    image_intensity_plot->setMarginGroup(QCP::msLeft|QCP::msRight, group);

//    connect(legend, SIGNAL(selectionChanged(QCPLegend::SelectableParts)), this, SLOT(slot_legen_change(QCPLegend::SelectableParts)));
}

void SingleWaveWidget::init_connection()
{
    connect(this, SIGNAL(mouseMove(QMouseEvent *)),this,SLOT(slot_mousemove_oncurve(QMouseEvent *)));
}


void SingleWaveWidget::slot_legen_change(QCPLegend::SelectableParts setlet)
{
}

void SingleWaveWidget::slot_mousemove_oncurve(QMouseEvent *event)
{
    QVector<double> vx,vy;
    double x = this->xAxis->pixelToCoord(event->pos().x());
    double y = this->yAxis->pixelToCoord(event->pos().y());
    vx<<0<<x<<this->xAxis->range().maxRange;
    vy<<y<<y<<y;

    this->graph(1)->setData(vx,vy);
    this->graph(1)->setPen(QPen(Qt::red));

    vx.clear();
    vy.clear();
    vx<<x<<x<<x;
    vy<<0<<y<<this->yAxis->range().maxRange;
    this->graph(2)->setData(vx,vy);
    this->graph(2)->setPen(QPen(Qt::red));
    this->replot();
}

CurveWidget::CurveWidget(QWidget *parent) :
    QWidget(parent)
{
    init_plot();
}

void CurveWidget::init_plot()
{
    m_vbox_layout = new QVBoxLayout();
//    m_image_intensity_plot = new SingleWaveWidget(this);
    m_ils_plot = new SingleWaveWidget(this);
//    m_vbox_layout->addWidget(m_image_intensity_plot);
    m_vbox_layout->addWidget(m_ils_plot);
    setLayout(m_vbox_layout);
}

}
