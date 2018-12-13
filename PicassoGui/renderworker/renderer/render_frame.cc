#include "render_frame.h"

#include <QtGui>
#include <QPoint>
#include <QLine>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QThreadPool>
#include <Qt>

#include "render_worker.h"
#include "render_bitmaps_to_image.h"
#include "render_palette.h"
#include "render_snap.h"
#include "render_defect_point.h"

#include "oasis_parser.h"
#include "oasis_layout.h"
#include "oasis_exception.h"
#include "layout_preprocess.h"


namespace render{

static const Oasis::float64 m_shift_unit = 0.1;

RenderFrame::RenderFrame(QWidget *parent):
    RenderObjectWidget(parent),
    m_image(0),
    m_pixmap(0),
    m_resolution(1.0),
    m_background(0),
    m_redraw_required(false),
    m_update_image(false)
{
    setBackgroundRole(QPalette::NoRole);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    connect(this, SIGNAL(signal_down_key_pressed()), this, SLOT(slot_down_shift()));
    connect(this, SIGNAL(signal_up_key_pressed()), this, SLOT(slot_up_shift()));
    connect(this, SIGNAL(signal_left_key_pressed()), this, SLOT(slot_left_shift()));
    connect(this, SIGNAL(signal_right_key_pressed()), this, SLOT(slot_right_shift()));

    init_viewport();

    setFocusPolicy(Qt::StrongFocus);

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
//    bool bright = background_color().green() > 128;
    size_t layers = m_layers_properties.size();
    size_t planes_per_layer = 3;

    m_view_ops.clear();
    m_view_ops.reserve(layers * planes_per_layer);
    render::ViewOp::Mode mode = render::ViewOp::Copy;
    int i = 0;
    for(std::vector<render::LayerProperties*>::iterator it = m_layers_properties.begin(); it != m_layers_properties.end(); it++, i++)
    {
        //contour fill vertex
        if((*it)->visible())
        {
            m_view_ops.push_back(render::ViewOp((*it)->frame_color(), mode, (*it)->line_style(), 0, render::ViewOp::Rect, (*it)->line_width()));
            m_view_ops.back().bitmap_index(i * planes_per_layer + 0);
            m_view_ops.push_back(render::ViewOp((*it)->fill_color(), mode, 0, (*it)->pattern()));
            m_view_ops.back().bitmap_index(i * planes_per_layer + 1);
            m_view_ops.push_back(render::ViewOp((*it)->fill_color(), mode, 0, (*it)->pattern()));
            m_view_ops.back().bitmap_index(i * planes_per_layer + 2);
        }
    }
}

void RenderFrame::start_render()
{
    size_t layers = m_layers_properties.size();
    size_t planes_per_layer = 3;

    size_t num_planes = layers * planes_per_layer;
    for(size_t i = 0; i < num_planes; i++)
    {
        m_buffer.push_back(new render::Bitmap(width(), height(), 1.0));
    }

    QThreadPool pool;
    pool.setMaxThreadCount(layers);

    m_vp.set_size(width(), height());
    Oasis::OasisBox box = m_vp.box();
    Oasis::OasisTrans vp_trans = m_vp.trans();
    unsigned int i = 0;
    for (std::vector<render::LayerProperties*>::iterator it = m_layers_properties.begin(); it != m_layers_properties.end(); it++, i++)
    {
        if((*it)->visible())
        {
            render::LayerMetaData l = (*it)->metadata();
            Oasis::LDType ld(l.get_layer_num(), l.get_data_type());
            render::LayoutView lv = m_layout_views[(*it)->view_index()];
            render::RenderWorker* worker = new render::RenderWorker(lv.get_layout(), -1, box, vp_trans, ld, m_buffer[0]->width(), m_buffer[0]->height(), m_buffer[0]->resolution(), m_buffer[i * planes_per_layer], m_buffer[i * planes_per_layer + 1], m_buffer[i * planes_per_layer + 2]);
            pool.start(worker);
        }
    }
    
}

void RenderFrame::prepare_drawing()
{
    if(m_redraw_required)
    {
        if(m_image)
        {
            delete m_image;
            m_image = 0;
        }

        m_image = new QImage(width(), height(), QImage::Format_RGB32);

        while(! m_buffer.empty())
        {
            if(m_buffer.back() != 0)
            {
                delete m_buffer.back();
            }
            m_buffer.pop_back();
        }

        if(m_image)
        {
            m_image->fill(QColor(255, 255, 255));
        }

        start_render();
        set_plane_width(width());
        set_plane_height(height());
        set_plane_resolution(1.0);
        m_redraw_required = false;
        m_update_image = true;
    }
}


void RenderFrame::paintEvent(QPaintEvent * e )
{
    prepare_drawing();
    if(m_image)
    {
        if(m_update_image)
        {
            m_update_image = false;
            bitmaps_to_image (m_view_ops, m_buffer, m_pattern, m_line_style, m_image, width(), height(), &m_mutex);

            if(m_pixmap)
            {
                delete m_pixmap;
                m_pixmap = 0;
            }
        }

        if(!m_pixmap || require_update_static())
        {
            if(m_pixmap)
            {
                delete m_pixmap;
                m_pixmap = 0;
            }

            clear_foreground();
            render_foreground(m_vp, this, true);

            m_pixmap = new QPixmap();

            if(foreground_bitmaps() > 0)
            {
                QImage repo_image (*m_image);
                bitmaps_to_image(foreground_view_ops(), foreground_bitmaps_vector(), m_pattern, m_line_style, &repo_image, width(), height(), &m_mutex);
                *m_pixmap = QPixmap::fromImage(repo_image);
            }
            else
            {
                *m_pixmap = QPixmap::fromImage(*m_image);
            }
        }

        QPainter painter(this);
        painter.drawPixmap(QPoint(0,0), *m_pixmap);

        clear_foreground();
        render_foreground(m_vp, this, false);
        if(foreground_bitmaps() > 0)
        {
            QImage repo_image(m_image->size().width(), m_image->size().height(), QImage::Format_ARGB32);
            repo_image.fill(0);
            bitmaps_to_image(foreground_view_ops(), foreground_bitmaps_vector(), m_pattern, m_line_style, &repo_image, width(), height(), &m_mutex);
            painter.drawPixmap(QPoint(0,0), QPixmap::fromImage(repo_image));
        }

        clear_foreground();

    }
    e->accept();
}


void RenderFrame::wheelEvent(QWheelEvent * e)
{
    Oasis::OasisPoint p = m_vp.trans().inverted().trans(Oasis::OasisPoint(e->pos().x(), m_vp.height() - 1 - e->pos().y()));
    Oasis::OasisBox vp_box = m_vp.box();
    int delta = e->delta();
    double step = 0.25;
    double f = 1;
    if(delta > 0)
    {
        f = 1.0 / (1.0 + step * (delta / 120.0));
    }
    else
    {
        f = 1.0 + step * (-delta / 120.0);
    }
    
    zoom_box(Oasis::OasisBox((p.x() - (Oasis::int64)((p.x() - vp_box.left()) * f)),
                             (p.y() - (Oasis::int64)((p.y() - vp_box.bottom()) * f)),
                             (p.x() - (Oasis::int64)((p.x() - vp_box.right()) * f)),
                             (p.y() - (Oasis::int64)((p.y() - vp_box.top()) * f))
                            ));


    e->ignore();
}


void RenderFrame::keyPressEvent(QKeyEvent * e)
{
    int key = e->key();
    if(key == Qt::Key_Down)
    {
        emit signal_down_key_pressed();
    }
    else if(key  == Qt::Key_Up)
    {
        emit signal_up_key_pressed();
    }
    else if(key == Qt::Key_Left)
    {
        emit signal_left_key_pressed();
    }
    else if(key == Qt::Key_Right)
    {
        emit signal_right_key_pressed();
    }

    e->accept();
}

void RenderFrame::mouseMoveEvent(QMouseEvent* e)
{
    QPoint p1 = e->pos();
    Oasis::OasisPoint physics_p = m_vp.trans().inverted().trans(Oasis::OasisPoint(p1.x(), m_vp.height() - 1 - p1.y()));
    emit signal_pos_updated(((double) physics_p.x() / render::dbu), ((double) physics_p.y() / render::dbu));
    QWidget::mouseMoveEvent(e);
    e->ignore();
}

void RenderFrame::resizeEvent(QResizeEvent * e)
{
    e->ignore();
    m_redraw_required = true;
    QWidget::resizeEvent(e);
}

void RenderFrame::init_viewport()
{
    m_vp.set_size(100, 100);
    m_vp.set_box(Oasis::OasisBox(0, 0, 100000, 100000));
}


const render::LayerProperties* RenderFrame::get_properties(int index) const
{
    return m_layers_properties[index];
}


void RenderFrame::set_properties(unsigned int index, const LayerProperties& lp)
{
    if(index >= layers_size())
    {
        return;
    }


    *m_layers_properties[index] = lp;
    m_layers_properties[index]->set_view(this);
    m_layers_properties[index]->set_layer_index(index);

    if(index == current_layer())
    {
        set_view_ops();
        m_redraw_required = true;
        update();
    }
}


Oasis::OasisBox RenderFrame::get_box() const
{
    return m_vp.box();
}


void RenderFrame::zoom_box(const Oasis::OasisBox& box)
{
    m_vp.set_box(box);
    m_redraw_required = true;

    slot_box_updated();

    update();
}

void RenderFrame::set_cursor_widget(QWidget *cursor_widget)
{
    QHBoxLayout *Hlayout = new QHBoxLayout(this);
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->setSpacing(0);
    Hlayout->addWidget(cursor_widget);

}


void RenderFrame::slot_down_shift()
{
    shift_view(1.0, 0, -m_shift_unit);
}


void RenderFrame::slot_up_shift()
{
    shift_view(1.0, 0, m_shift_unit);
}


void RenderFrame::slot_left_shift()
{
    shift_view(1.0, -m_shift_unit, 0);
}

void RenderFrame::slot_right_shift()
{
    shift_view(1.0, m_shift_unit, 0);
}

void RenderFrame::slot_zoom_in()
{
    shift_view(0.8, 0, 0);
}

void RenderFrame::slot_zoom_out()
{
    shift_view(1.2, 0, 0);
}

void RenderFrame::slot_get_snap_pos(QPoint pos, int mode)
{
    if(mode == 1 || mode == 2)
    {
        std::pair<QPoint, std::pair<double, double> > result = get_snap_point(pos);
        emit signal_get_snap_pos(result.first, result.second.first, result.second.second, mode);
    }
    else if(mode == 3)
    {

    }
}

void RenderFrame::shift_view(Oasis::float64 scale, Oasis::float64 dx, Oasis::float64 dy)
{
    Oasis::OasisBox box = m_vp.box();
    Oasis::OasisPointF center((box.right() - box.left()) / 2 + box.left(), (box.top() - box.bottom()) / 2 + box.bottom());

    Oasis::OasisPointF new_center = center + Oasis::OasisPointF((Oasis::float64)dx * box.width(), (Oasis::float64) dy * box.height());

    Oasis::float64 width = box.width() * scale;
    Oasis::float64 height = box.height() * scale;

    Oasis::OasisPoint p1 = Oasis::from_float(Oasis::OasisPointF(new_center.x() - 0.5 * width, new_center.y() - 0.5 * height));
    Oasis::OasisPoint p2 = Oasis::from_float(Oasis::OasisPointF(new_center.x() + 0.5 * width, new_center.y() + 0.5 * height));
    zoom_box(Oasis::OasisBox(p1.x(), p1.y(), p2.x(), p2.y()));

}

static Oasis::OasisBox calculate_box(double x, double y, const Oasis::OasisBox& box)
{
    Oasis::OasisPoint center((Oasis::int64) x, (Oasis::int64) y);
    Oasis::int64 half_width = Oasis::int64(box.width() * 0.5);
    Oasis::int64 half_height = Oasis::int64(box.height() * 0.5);
    Oasis::OasisBox new_box(center.x() - half_width, center.y() - half_height, center.x() + half_width, center.y() + half_height);
    return new_box;
}

void RenderFrame::set_defect_point(double x, double y)
{
    for(RenderObjectWidget::object_iter it = begin_object(); it != end_object(); it++)
    {
        render::DefectPoint* point = 0;
        if ((point = dynamic_cast<DefectPoint*> (*it)) != 0)
        {
            point->set_point(x, y);
            center_at_point(x, y);
            return;
        }
    }
    DefectPoint* point = new DefectPoint(x, y, this, true);
    point->set_point(x, y);

    center_at_point(x, y);
}

void RenderFrame::center_at_point(double x, double y)
{
    Oasis::OasisBox box = m_vp.box();
    zoom_box(calculate_box(x * render::dbu, y * render::dbu, box));
}


void RenderFrame::zoom_center(int x, int y)
{
    if( x < width() && x > 0 && y < height() && y > 0)
    {
        Oasis::OasisPoint point = m_vp.trans().inverted().trans(Oasis::OasisPoint(x, m_vp.height() - 1 - y));
        center_at_point((double)point.x() / render::dbu, (double)point.y() / render::dbu );
    }
}

void RenderFrame::add_layout_view(LayoutView &view)
{
    load_layout_view(view, true);
}



void RenderFrame::detach_layout_view(LayoutView& view)
{

}

render::LayoutView RenderFrame::load_file(std::string file_name, std::string prep_dir, bool add_layout_view)
{

    Oasis::OasisLayout* layout = new Oasis::OasisLayout(file_name);

    render::LayoutView lv;
    lv.set_layout(layout);
    lv.set_widget(this);

    try
    {
        Oasis::OasisParser parser;
        parser.ImportFile(layout);
    }
    catch (Oasis::OasisException& e)
    {
        std::string prep_file_name = Oasis::preprocessLayout(file_name, prep_dir, false, Oasis::prep_options());
        delete layout;
        layout = new Oasis::OasisLayout(prep_file_name);
        Oasis::OasisParser parser;
        parser.ImportFile(layout);
    }

    if(!add_layout_view)
    {
        clear_layout_view();
    }

    int lv_index = layout_views_size();
    set_layout_view(lv, lv_index);

    std::set<std::pair<int, int> >layers;
    layout->getLayers(layers);

    create_and_initial_layer_properties(lv_index, layers);

    Oasis::OasisBox box = layout->get_bbox();
    Oasis::OasisPoint center(box.left() + (box.right() - box.left()) / 2, box.bottom() + (box.top() - box.bottom()) / 2 );
    Oasis::OasisBox new_box = calculate_box(center.x(), center.y(), m_vp.box());
    m_vp.set_box(new_box);

    update_view();
    return lv;
}

void RenderFrame::load_layout_view(render::LayoutView& lv, bool add_layout_view)
{
    int lv_index = 0;
    try
    {
        if(!add_layout_view)
        {
            clear_layout_view();
        }

        lv_index = layout_views_size();
        set_layout_view(lv, lv_index);
        lv.set_widget(this);
    }
    catch(...)
    {
        throw;
    }

    std::set<std::pair<int,int> > layers;
    Oasis::OasisLayout* layout = lv.get_layout();
    layout->getLayers(layers);

    create_and_initial_layer_properties(lv_index, layers);

    update_view();
}

const LayoutView& RenderFrame::get_layout_view(int index) const
{
    if(index > 0 && index < layout_views_size())
    {
        return m_layout_views[index];
    }
    else
    {
        return m_layout_views[0];
    }
}

void RenderFrame::set_layout_view(render::LayoutView& lv, int lv_index)
{
    while(lv_index >= layout_views_size())
    {
        m_layout_views.push_back(render::LayoutView());
    }
    m_layout_views[lv_index] = lv;
    lv.set_index(lv_index);
}

void RenderFrame::create_and_initial_layer_properties(int lv_index, std::set<std::pair<int,int> >& layers)
{
    int i = 0;
    for(std::set<std::pair<int,int> >::iterator it = layers.begin(); it != layers.end(); it++, i++)
    {
        int layer_num = it->first;
        int data_type = it->second;
        Oasis::OasisLayout* layout = m_layout_views[lv_index].get_layout();
        m_layers_properties.push_back(new render::LayerProperties());
        m_layers_properties.back()->set_view_index(lv_index);
        m_layers_properties.back()->set_view(this);
        m_layers_properties.back()->set_metadata(layout->getLayerName(layer_num, data_type), layer_num, data_type);
        m_layers_properties.back()->set_layer_index(std::distance(layers.begin(), it));
        m_layers_properties.back()->set_fill_color(render::color_table[i]);
        m_layers_properties.back()->set_frame_color(render::color_table[i]);
        m_layers_properties.back()->set_pattern(1);
        m_layers_properties.back()->set_line_width(1);
        if(i == render::color_table_size - 1)
        {
            i = -1;
        }
    }
}

void RenderFrame::update_view()
{
    set_view_ops();
}


void RenderFrame::erase_layout_view(int index)
{
    if((size_t)index >= m_layout_views.size())
    {
        return ;
    }

    std::vector<render::LayoutView>::iterator it = m_layout_views.begin() + index;
    m_layout_views.erase(it);

    for(size_t i = 0; i < layers_size(); i++)
    {
        if(m_layers_properties[i]->view_index() > index)
        {
            m_layers_properties[i]->set_view_index(m_layers_properties[i]->view_index() - 1);
        }
        else if(m_layers_properties[i]->view_index() == index)
        {
            m_layers_properties[i]->set_view_index(-1);
            m_layers_properties[i]->set_view((RenderFrame*) 0);
            delete_layer(i);
            i--;
        }
    }
}

void RenderFrame::clear_layout_view()
{
    while(layers_size() > 0)
    {
        delete_layer(layers_size() - 1);
    }
    m_layout_views.clear();

}

void RenderFrame::delete_layer(int index)
{
    delete m_layers_properties[index];
    m_layers_properties.erase(m_layers_properties.begin() + index);
}

void RenderFrame::slot_box_updated()
{
    Oasis::OasisBox physical_box(m_vp.box());
    emit signal_box_updated((double)physical_box.left() / render::dbu,
                            (double)physical_box.bottom() /render::dbu,
                            (double)physical_box.right() / render::dbu,
                            (double)physical_box.top() / render::dbu);
}

std::pair<QPoint, std::pair<double,double> > RenderFrame::get_snap_point(QPoint p1)
{
    Oasis::OasisPoint p = get_trans().inverted().trans(Oasis::OasisPoint(p1.x(), m_vp.height() - 1- p1.y()));
    int snap_range = 5;
    std::pair<bool,Oasis::OasisPoint> result = snap_point(this, p, snap_range);
    if(result.first)
    {
        Oasis::OasisPoint snap_phy_p = result.second;
        Oasis::OasisPoint snap_bitmap_p = get_trans().trans(snap_phy_p);
        QPoint snap_pix_p(snap_bitmap_p.x(), m_vp.height() - 1 - snap_bitmap_p.y());
        return std::make_pair(snap_pix_p, std::make_pair((double)snap_phy_p.x() / render::dbu, (double)snap_phy_p.y() / render::dbu));
    }
    else{
        return std::make_pair(p1, std::make_pair((double)p.x() / render::dbu, (double) p.y() / render::dbu));
    }
}

}
