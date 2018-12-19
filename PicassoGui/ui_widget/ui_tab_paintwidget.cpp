#include "ui_tab_paintwidget.h"
namespace ui {

TabPaintWidget::TabPaintWidget()
{
    sizePolicy().setVerticalPolicy(QSizePolicy::Ignored);
    setTabsClosable(true);
    setMouseTracking(true);
}


void TabPaintWidget::slot_close_tab(QString fileName)
{
    int index = string_to_index(fileName);
    removeTab(index);
}

void TabPaintWidget::creat_canvas(QString fileName)
{
    ScaleFrame *scaleframe = new ScaleFrame(this);
    addTab(scaleframe, fileName);
    m_filename_list.append(fileName);
}

render::LayoutView TabPaintWidget::load_file(const QString & filename, const QString &dirpath, bool add_layout_view)
{
    scameframe->load_file(filename, dirpath, add_layout_view);
}

int TabPaintWidget::string_to_index(QString fileName)
{
    return m_filename_list.indexOf(fileName);
}

}
