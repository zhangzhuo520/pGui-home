#include "ui_tab_paintwidget.h"
namespace ui {

TabPaintWidget::TabPaintWidget(QWidget *parent)
{
    setParent(parent);
    sizePolicy().setVerticalPolicy(QSizePolicy::Ignored);
    setTabsClosable(true);
    setMouseTracking(true);
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

void TabPaintWidget::append_canvas(QString fileName)
{
    if (m_scaleframe_vector.isEmpty())
    {
        return;
    }
    addTab(m_scaleframe_vector.at(m_scaleframe_vector.count() - 1), fileName);
    setCurrentIndex(count() - 1);
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

}
