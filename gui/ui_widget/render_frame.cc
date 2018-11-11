#include "render_frame.h"

#include <QtGui>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include "render_bitmaps_to_image.h"

namespace render{
RenderFrame::RenderFrame(QWidget *parent) :
    QFrame(parent),
    m_image(0),
    m_pixmap(0),
    m_resolution(1.0),
    m_background(0)
{
    setBackgroundRole(QPalette::NoRole);
    
    setAttribute(Qt::WA_NoSystemBackground);
    set_view_ops();
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
}

void RenderFrame::set_pattern(const render::Pattern &p)
{
    if(m_pattern  != p)
    {
        m_pattern = p;
        update(); 
    }
}

void RenderFrame::set_view_ops()
{
    bool bright = background_color().green() > 128;
    size_t layers = 4;
    
    m_view_ops.reserve(layers);
    render::ViewOp::Mode mode = render::ViewOp::Copy;
    color_t fill_color, frame_color;
    
    int pattern = 0;
    int line_style = 1;
    
    int line_width = 1;

    frame_color = fill_color = 0x00555555;
    
    m_view_ops.push_back(render::ViewOp(fill_color, mode, 0, pattern));
    m_view_ops.push_back(render::ViewOp(frame_color, mode, line_style, 0, render::ViewOp::Rect, line_width));

    pattern = 5;
    line_style = 5;
    frame_color = fill_color = 0x00ffffff;

    m_view_ops.push_back(render::ViewOp(fill_color, mode, 0, pattern));
    m_view_ops.push_back(render::ViewOp(frame_color, mode, line_style, 0, render::ViewOp::Rect, line_width));

}


void RenderFrame::init()
{

    render::RenderEdge edge1(50.0, 100.0, 50.0, 200.0);
    render::RenderEdge edge2(50.0, 200.0, 100.0,200.0);
    render::RenderEdge edge3(100.0,200.0, 100.0, 100.0);
    render::RenderEdge edge4(100.0,100.0, 50.0, 100.0);
    std::vector<render::RenderEdge> vec1;
    vec1.push_back(edge1);
    vec1.push_back(edge2);
    vec1.push_back(edge3);
    vec1.push_back(edge4);
    
    render::RenderEdge edge5(75.0,  150.0, 75.0, 250.0);
    render::RenderEdge edge6(75.0,  250.0, 125.0, 250.0);
    render::RenderEdge edge7(125.0, 250.0, 125.0, 150.0);
    render::RenderEdge edge8(125.0, 150.0, 75.0, 150.0);
    std::vector<render::RenderEdge> vec2;
    vec2.push_back(edge5);
    vec2.push_back(edge6);
    vec2.push_back(edge7);
    vec2.push_back(edge8);

    for(int i = 0; i < 4; i++)
    {
        m_buffer.push_back(new render::Bitmap(width(), height(), 1.0));
    }

    m_buffer[0]->render_fill_ortho(vec1);
    m_buffer[1]->render_contour_ortho(vec1);

    m_buffer[2]->render_fill_ortho(vec2);
    m_buffer[3]->render_contour_ortho(vec2);
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

}
