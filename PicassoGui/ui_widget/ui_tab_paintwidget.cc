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


void TabPaintWidget::slot_close_tab(QString fileName)
{
    fileName = database_to_oas(fileName);
    int index = string_to_index(fileName);
    if (index == -1)
    {
        return;
    }
    removeTab(index);
    m_scaleframe_vector.remove(index);
}

void TabPaintWidget::creat_canvas()
{
    ScaleFrame* scaleframe = new ScaleFrame(this);
    m_scaleframe_vector.append(scaleframe);
}

void TabPaintWidget::slot_show_measure_table()
{
    if (m_scaleframe_vector.isEmpty())
    {
        return;
    }
    QList <LineData> line_list = m_scaleframe_vector[currentIndex()]->get_measure_line_list();

    if (line_list.isEmpty())
    {
        return;
    }
    m_measure_table->set_line_list(line_list);
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
    addTab(m_scaleframe_vector.at(m_scaleframe_vector.count() - 1), fileName);
    setCurrentIndex(count() - 1);
}

void TabPaintWidget::set_active_widget(QString filename)
{
    int index = string_to_index(filename);
    setCurrentIndex(index);
}

render::LayoutView TabPaintWidget::load_file(const QString & filename, const QString &dirpath, bool add_layout_view)
{
    if (m_scaleframe_vector.isEmpty())
    {
        return render::LayoutView();
    }
    return m_scaleframe_vector.at(m_scaleframe_vector.count() - 1)->load_file(filename, dirpath, add_layout_view);
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

QString TabPaintWidget::database_to_oas(QString m_filename)
{
    if(m_filename.right(3) == ".db")
    {
        m_filename = m_filename.left(m_filename.size() - 9);
        m_filename =  m_filename + "DefectFile.oas";
    }
    return m_filename;
}

void TabPaintWidget::init_measure_table()
{
    m_measure_dockwidget = new DockWidget("Measure Table", m_mainwindow, Qt::WindowFlags(Qt::RightDockWidgetArea));
    m_measure_table = new MeasureTable(m_mainwindow);
    m_measure_dockwidget->setWidget(m_measure_table);
    m_measure_dockwidget->setFloating(true);
    m_measure_dockwidget->move(m_mainwindow->width()/ 2, m_mainwindow->height() / 2);
    m_measure_dockwidget->hide();
    connect(m_measure_table, SIGNAL(singal_set_line_list(const QList <LineData>&)), this, SLOT(slot_set_line_list(const QList <LineData>&)));
}

}
