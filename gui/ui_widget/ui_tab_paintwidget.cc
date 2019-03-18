#include "ui_tab_paintwidget.h"
namespace ui {
TabPaintWidget::TabPaintWidget(QWidget *parent)
{
    m_mainwindow = parent;
    sizePolicy().setVerticalPolicy(QSizePolicy::Ignored);
    setTabsClosable(true);
    setMouseTracking(true);
    init_measure_table();
}

ScaleFrame *TabPaintWidget::get_scaleframe(int index)
{
    return m_scaleframe_vector.at(index);
}

TabPaintWidget::~TabPaintWidget()
{

}

void TabPaintWidget::get_canvas_coord(double * left, double *right, double *bottom, double *top)
{
    if (currentIndex() > -1 && currentIndex() < m_scaleframe_vector.count())
    {
        m_scaleframe_vector[currentIndex()]->get_canvas_coord(left, right, bottom, top);
    }
    else
    {
        logger_console << "canvas tab error !";
        return;
    }
}

void TabPaintWidget::slot_close_tab(int index)
{
    if(index >= m_scaleframe_vector.count())
    {
        return ;
    }

    ScaleFrame* frame = m_scaleframe_vector.at(index);
    m_scaleframe_vector[index] = NULL;
    m_scaleframe_vector.erase(m_scaleframe_vector.begin() + index);

    removeTab(index);
    delete frame;

    update_measuretable_data();
}


ScaleFrame* TabPaintWidget::creat_canvas()
{
    ScaleFrame* scaleframe = new ScaleFrame(this);
    m_scaleframe_vector.append(scaleframe);
    init_connection();
    connect_layer_widget();
    return scaleframe;
}

void TabPaintWidget::slot_show_measure_table()
{
    if (m_scaleframe_vector.isEmpty())
    {
        return;
    }

    QList <LineData> line_list = m_scaleframe_vector[currentIndex()]->get_measure_line_list();

    m_measure_table->slot_set_line_list(line_list);
    m_measure_dockwidget->show();
}

void TabPaintWidget::slot_set_line_list(const QList<LineData> & line_list)
{
    m_scaleframe_vector[currentIndex()]->set_measure_line_list(line_list);
}

void TabPaintWidget::append_canvas(QString fileName)
{
    if (m_scaleframe_vector.isEmpty())
    {
        return;
    }

    addTab(m_scaleframe_vector[m_scaleframe_vector.count() - 1], fileName);
    setCurrentIndex(count() - 1);
    QString tab_tool_tip;
    QVector<QString> result = m_scaleframe_vector.at(m_scaleframe_vector.count() - 1)->get_file_name_list();
    tab_tool_tip.append(QString("Single:"));
    tab_tool_tip.append(result.at(0));
    setTabToolTip(m_scaleframe_vector.count() - 1, tab_tool_tip);
}

void TabPaintWidget::set_active_widget(render::RenderFrame* frame)
{
    for(int i = 0; i < m_scaleframe_vector.count(); i++)
    {
        if(frame == m_scaleframe_vector.at(i)->getRenderFrame())
        {
            setCurrentIndex(i);
            return ;
        }
    }
}

void TabPaintWidget::update_measuretable_data()
{
    if (currentIndex() >= 0)
    {
        QList <LineData> line_list = m_scaleframe_vector[currentIndex()]->get_measure_line_list();
        m_measure_table->slot_set_line_list(line_list);
    }
    else
    {
        m_measure_table->only_clear_table_data();
    }
}

int TabPaintWidget::string_to_index(QString fileName)
{
    for (int i = 0; i < m_scaleframe_vector.count(); i ++)
    {
        if (fileName == m_scaleframe_vector.at(i)->get_file_name())
        {
            return i;
        }
    }
    return -1;
}

void TabPaintWidget::init_measure_table()
{
    m_measure_dockwidget = new DockWidget("Measure Table", m_mainwindow, Qt::WindowFlags(Qt::LeftDockWidgetArea));
    m_measure_table = new MeasureTable(m_mainwindow);
    m_measure_dockwidget->setWidget(m_measure_table);
    m_measure_dockwidget->setFloating(true);
    connect(m_measure_table, SIGNAL(signal_set_line_list(const QList <LineData>&)), this, SLOT(slot_set_line_list(const QList <LineData>&)));
    connect(this, SIGNAL(signal_set_line_list(const QList <LineData>&)), m_measure_table, SLOT(slot_set_line_list(const QList <LineData>&)));
}

void TabPaintWidget::init_connection()
{
    connect(m_scaleframe_vector.at(m_scaleframe_vector.count() - 1), SIGNAL(signal_update_measuretable()), this, SLOT(slot_show_measure_table()));
}

void TabPaintWidget::connect_layer_widget()
{
    connect(m_scaleframe_vector.at(m_scaleframe_vector.count() - 1), SIGNAL(signal_layout_view_changed(render::RenderFrame*)), this, SLOT(slot_layout_view_changed(render::RenderFrame*)));
}

void TabPaintWidget::slot_layout_view_changed(render::RenderFrame* frame)
{
    emit signal_layout_view_changed(frame);
}

QVector<render::RenderFrame*> TabPaintWidget::get_render_frame_list()
{
    QVector<render::RenderFrame*> result;
    for(int i = 0; i < m_scaleframe_vector.count(); i++)
    {
        result.push_back(m_scaleframe_vector.at(i)->getRenderFrame());
    }
    return result;
}

}
