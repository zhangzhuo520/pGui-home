#include "render_frame.h"

#include <QtGui>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include "render_worker.h"
#include "render_viewport.h"
#include "render_bitmaps_to_image.h"
#include "render_palette.h"

#include "OasisParser.h"
#include "OasisLayout.h"


namespace render{
RenderFrame::RenderFrame(QWidget *parent) :
    QFrame(parent),
    m_image(0),
    m_pixmap(0),
    m_resolution(1.0),
    m_background(0),
    m_layout(0)
{
    setBackgroundRole(QPalette::NoRole);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    load_file("/home/zhaofeng/picasso/build/test/hierarchy.prep.oas");
    set_view_ops();
    set_viewport();
}


RenderFrame::~RenderFrame()
{
    if(m_image)
    {
        delete m_image;
        m_image = 0;
    }

    if(m_pixmap)
    {
        delete m_pixmap;
        m_pixmap = 0;
    }

    if(m_buffer.size() != 0)
    {
        for(size_t i = 0; i < m_buffer.size(); i++)
        {
            delete m_buffer[i];
            m_buffer[i] = 0;
        }
    }
    delete m_layout;
}

void RenderFrame::set_pattern(const render::Pattern &p)
{
    if(m_pattern  != p)
    {
        m_pattern = p;
        update(); 
    }
}

Oasis::OasisLayout* RenderFrame::load_file(std::string file_name)
{
    m_layout = new Oasis::OasisLayout(file_name);
    Oasis::OasisParser parser;
    parser.ImportFile(m_layout);
    std::set<std::pair<int, int> >layers;
    m_layout->getLayers(layers);

    int j = 0;
    for(std::set<std::pair<int,int> >::iterator it = layers.begin(); it != layers.end(); it++, j++)
    {
        int layer_num = it->first;
        int data_type = it->second;
        m_layers_properties.push_back(render::LayerProperties());
        m_layers_properties.back().set_metadata(m_layout->getLayerName(layer_num, data_type), layer_num, data_type);
        m_layers_properties.back().set_layer_index(std::distance(layers.begin(), it));
        m_layers_properties.back().set_fill_color(render::color_table[j]);
        m_layers_properties.back().set_frame_color(render::color_table[j]);
        m_layers_properties.back().set_pattern(1);
        if(j == render::color_table_size - 1)
        {
            j = -1;
        }
    }
    return m_layout;
}

void RenderFrame::set_view_ops()
{
//    bool bright = background_color().green() > 128;
    size_t layers = m_layers_properties.size();
    size_t planes_per_layer = 3;

    m_view_ops.reserve(layers * planes_per_layer);
    render::ViewOp::Mode mode = render::ViewOp::Copy;
    int line_width = 1;

    for(std::vector<render::LayerProperties>::iterator it = m_layers_properties.begin(); it != m_layers_properties.end(); it++)
    {
        //contour fill vertex
        m_view_ops.push_back(render::ViewOp(it->frame_color(), mode, it->line_style(), 0, render::ViewOp::Rect, line_width));
        m_view_ops.push_back(render::ViewOp(it->fill_color(), mode, 0, it->pattern()));
        m_view_ops.push_back(render::ViewOp(it->fill_color(), mode, 0, it->pattern()));
    }
}

void RenderFrame::init()
{
    size_t layers = m_layers_properties.size();
    size_t planes_per_layer = 3;

    size_t num_planes = layers * planes_per_layer;
    for(size_t i = 0; i < num_planes; i++)
    {
        m_buffer.push_back(new render::Bitmap(width(), height(), 1.0));
    }
/*
    Oasis::OasisBox target_box(0, 0, 100000, 100000);
    render::Viewport vp((unsigned int)width(), (unsigned int)height(), target_box);
    Oasis::OasisBox box = vp.box();
    Oasis::OasisTrans vp_trans = vp.trans();*/

    m_vp.set_size(width(), height());
    Oasis::OasisBox box = m_vp.box();
    Oasis::OasisTrans vp_trans = m_vp.trans();
    unsigned int i = 0;
    for (std::vector<render::LayerProperties>::const_iterator it = m_layers_properties.begin(); it != m_layers_properties.end(); it++, i++)
    {
        render::LayerMetaData l = it->metadata();
        Oasis::LDType ld(l.get_layer_num(), l.get_data_type());
        render::RenderWorker worker(m_layout, -1, box, vp_trans, ld, m_buffer[0]->width(), m_buffer[0]->height(), m_buffer[0]->resolution(), m_buffer[i * planes_per_layer], m_buffer[i * planes_per_layer + 1], m_buffer[i * planes_per_layer + 2]);
        worker.run();
    }
    
}

void RenderFrame::prepare_drawing()
{
    if(m_image)
    {
        m_image->fill(palette().color(QPalette::Normal, QPalette::Background));
    }
}


void RenderFrame::paintEvent(QPaintEvent *)
{

    if(!m_image)
    {
        delete m_image;
        m_image = 0;
    }

    m_image = new QImage(width(), height(), QImage::Format_RGB32);

    if(!m_pixmap)
    {
        delete m_pixmap;
        m_pixmap = 0;
    }

    while(! m_buffer.empty())
    {
        if(m_buffer.back() != 0)
        {
            delete m_buffer.back();
        }
        m_buffer.pop_back();
    }

    init();
    prepare_drawing();

    bitmaps_to_image (m_view_ops, m_buffer, m_pattern, m_line_style, m_image, width(), height(), &m_mutex);
    m_pixmap = new QPixmap();
    *m_pixmap = QPixmap::fromImage(*m_image);
    QPainter painter(this);
    painter.drawPixmap(QPoint(0,0), *m_pixmap);

}

void RenderFrame::zoom_box(const Oasis::OasisBox& box)
{
    m_vp.set_box(box);
    update();
}

void RenderFrame::wheelEvent(QWheelEvent * e)
{
    e->ignore();

    bool hor_direction = (e->orientation() == Qt::Horizontal);
    Oasis::OasisPoint p = m_vp.trans().inverted().trans(Oasis::OasisPoint(e->pos().x(), e->pos().y()));
    Oasis::OasisBox vp_box = m_vp.box();
    int delta = e->delta();

    double step = 0.25;
    double f = 1;
    if(delta > 0)
    {
        f = 1.0 + step * (-delta / 120.0);
    }
    else
    {
        f = 1.0 / (1.0 + step * (delta / 120.0));
    }
    
    zoom_box(Oasis::OasisBox((p.x() - (p.x() - vp_box.left()) * f),
                             (p.y() - (p.y() - vp_box.bottom()) * f),
                             (p.x() - (p.x() - vp_box.right()) * f),
                             (p.y() - (p.y() - vp_box.top()) * f)
                            ));

}

void RenderFrame::set_viewport()
{
    m_vp.set_size(100, 100);
    m_vp.set_box(Oasis::OasisBox(0, 0, 100000, 100000));
}

const std::vector<render::LayerProperties>& RenderFrame::get_properties_list() const
{
    return m_layers_properties;
}

void RenderFrame::set_properties(const LayerProperties& lp)
{
    int index = lp.layer_index();
    m_layers_properties[index] = lp;
    update();
}

void RenderFrame::set_cursor_widget(QWidget *cursor_widget)
{
    QHBoxLayout *Hlayout = new QHBoxLayout(this);
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->setSpacing(0);
    Hlayout->addWidget(cursor_widget);
}


//Oasis::OasisBox& RenderFrame::get_box()
//{
//    return m_vp.box();
//}

}
