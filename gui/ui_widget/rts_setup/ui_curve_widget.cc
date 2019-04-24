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
//    for (double i = 0; i < 100; )
//    {
//        m_ai_x_vector.append(i);
//        m_ri_x_vector.append(i);
//        m_ei_x_vector.append(i);

//        double data = (double)(rand()/(double)RAND_MAX);
//        m_ai_y_vector.append(data);
//        data = (double)(rand()/(double)RAND_MAX);
//        m_ri_y_vector.append(data);
//        data = (double)(rand()/(double)RAND_MAX);
//        m_ei_y_vector.append(data);
//        i = i + 0.01;
//    }

    for(int i=0;i<100;i++)
    {
        double x = i / 5.0 - 10;
        double y = x * x * x;
        m_ai_x_vector.append(x);
        m_ai_y_vector.append(y);
        m_ri_x_vector.append(x);
        m_ri_y_vector.append(y);
        m_ei_x_vector.append(x);
        m_ei_y_vector.append(y);
    }
}

void SingleWaveWidget::paint_curve()
{
    setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    plotLayout()->clear();

    image_intensity_plot = new QCPAxisRect(this);
    plotLayout()->addElement(0, 0, image_intensity_plot);
    image_intensity_plot->axis(QCPAxis::atBottom)->setLabel(QString("Distance(nm)"));
    image_intensity_plot->axis(QCPAxis::atLeft)->setLabel(QString("Norm intensity"));

    plotLayout()->setRowSpacing(0);
    image_intensity_plot->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msAll);
    image_intensity_plot->setMargins(QMargins(0, 0, 0, 0));


    m_ri_curve = new QCPCurve(image_intensity_plot->axis(QCPAxis::atBottom), image_intensity_plot->axis(QCPAxis::atLeft));
    m_ri_curve->setName("RiImage");
    m_ri_curve->setPen(QColor(180, 90, 90));

    m_ei_curve = new QCPCurve(image_intensity_plot->axis(QCPAxis::atBottom), image_intensity_plot->axis(QCPAxis::atLeft));
    m_ei_curve->setName("EiImage");
    m_ei_curve->setPen(QColor(220, 40, 40));

    for (int i = 0; i < 100; i ++)
    {
        m_ri_curve->setData(m_ri_x_vector, m_ri_y_vector);
        m_ei_curve->setData(m_ei_x_vector, m_ei_y_vector);
    }

    ils_plot = new QCPAxisRect(this);
//    ils_plot->
    plotLayout()->addElement(1, 0, ils_plot);
    ils_plot->axis(QCPAxis::atBottom)->setLabel(QString("Distance(nm)"));
    ils_plot->axis(QCPAxis::atLeft)->setLabel(QString("ILS(um)"));

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
    m_top_cursor_x_curve = new QCPCurve(image_intensity_plot->axis(QCPAxis::atBottom), image_intensity_plot->axis(QCPAxis::atLeft));
    m_top_cursor_y_curve = new QCPCurve(image_intensity_plot->axis(QCPAxis::atBottom), image_intensity_plot->axis(QCPAxis::atLeft));
    m_bottom_cursor_x_curve = new QCPCurve(ils_plot->axis(QCPAxis::atBottom), ils_plot->axis(QCPAxis::atLeft));
    m_bottom_cursor_y_curve = new QCPCurve(ils_plot->axis(QCPAxis::atBottom), ils_plot->axis(QCPAxis::atLeft));
}

void SingleWaveWidget::init_connection()
{
    connect(this, SIGNAL(mouseMove(QMouseEvent *)),this, SLOT(slot_mousemove_oncurve(QMouseEvent *)));
}

void SingleWaveWidget::slot_legen_change(QCPLegend::SelectableParts setlet)
{
    Q_UNUSED(setlet);
}

void SingleWaveWidget::slot_mousemove_oncurve(QMouseEvent *event)
{
    QVector<double> vx,vy;
    double x = image_intensity_plot->axis(QCPAxis::atBottom)->pixelToCoord(event->pos().x());
    double y = image_intensity_plot->axis(QCPAxis::atLeft)->pixelToCoord(event->pos().y());

    vx<<x<<x<<x;
    vy << -yAxis->range().maxRange<< 0 << yAxis->range().maxRange;

    m_top_cursor_y_curve->setData(vx,vy);
    m_top_cursor_y_curve->setPen(QPen(Qt::red));

    vx.clear();
    vy.clear();

    vx << -xAxis->range().maxRange << 0 << xAxis->range().maxRange;
    vy << y << y<< y;

    m_top_cursor_x_curve->setData(vx,vy);
    m_top_cursor_x_curve->setPen(QPen(Qt::red));

    vx.clear();
    vy.clear();

    x = ils_plot->axis(QCPAxis::atBottom)->pixelToCoord(event->pos().x());
    y = ils_plot->axis(QCPAxis::atLeft)->pixelToCoord(event->pos().y());
    vx<< -xAxis->range().maxRange << 0 << xAxis->range().maxRange;
    vy<<y<<y<<y;
    m_bottom_cursor_x_curve->setData(vx,vy);
    m_bottom_cursor_x_curve->setPen(QPen(Qt::red));

    vx.clear();
    vy.clear();

    vx<<x<<x<<x;
    vy<<-yAxis->range().maxRange<< 0 <<yAxis->range().maxRange;
    m_bottom_cursor_y_curve->setData(vx,vy);
    m_bottom_cursor_y_curve->setPen(QPen(Qt::red));



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
    m_wave_widget = new SingleWaveWidget(this);

    m_hboxlayout = new QHBoxLayout;
    m_focuson_label = new QLabel(this);
    m_focuson_label->setText("Focus on :");
    m_ai_button = new QRadioButton(this);
    m_ai_button->setText("Ai Image");
    m_ri_button = new QRadioButton(this);
    m_ri_button->setText("Ri Image");
    m_ei_button = new QRadioButton(this);
    m_ei_button->setText("Ei Image");
    m_hboxlayout->addWidget(m_focuson_label);
    m_hboxlayout->addWidget(m_ai_button);
    m_hboxlayout->addWidget(m_ri_button);
    m_hboxlayout->addWidget(m_ei_button);
    m_hboxlayout->addWidget(new QLabel());
    m_hboxlayout->setStretch(0, 1);
    m_hboxlayout->setStretch(1, 1);
    m_hboxlayout->setStretch(2, 1);
    m_hboxlayout->setStretch(3, 1);
    m_hboxlayout->setStretch(4, 5);

    m_vbox_layout->addWidget(m_wave_widget);
    m_vbox_layout->addLayout(m_hboxlayout);

    m_vbox_layout->setStretch(0, 10);
    m_vbox_layout->setStretch(1, 1);
    setLayout(m_vbox_layout);
}
}
