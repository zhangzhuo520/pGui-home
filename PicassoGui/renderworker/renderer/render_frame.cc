#include "render_frame.h"

#include <QtGui>
#include <QPoint>
#include <QPointF>
#include <QLine>
#include <QList>
#include <QPair>
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
#include <QDebug>

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

    m_background_color = QColor(255,255,255);
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
    size_t layers = m_layers_properties.size();
    size_t planes_per_layer = 3;

    m_view_ops.clear();
    m_view_ops.reserve(layers * planes_per_layer);
    render::ViewOp::Mode mode = render::ViewOp::Copy;
    int i = 0;
    for(size_t j = 0; j < m_layout_views.size(); j++)
    {
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
    Oasis::OasisBoxF micron_box_f = m_vp.box();
    Oasis::OasisTrans vp_trans = m_vp.trans();
    unsigned int i = 0;
    for(size_t j = 0 ; j < m_layout_views.size(); j++)
    {
        for (std::vector<render::LayerProperties*>::iterator it = m_layers_properties.begin(); it != m_layers_properties.end(); it++, i++)
        {
            if((*it)->visible())
            {
                Oasis::float64 dbu = m_layout_views[j].get_dbu();

                Oasis::OasisTrans dbu_trans(false, 0.0, dbu, Oasis::OasisPointF(0.0, 0.0));
                Oasis::OasisBoxF dbu_box_f = micron_box_f.transed(dbu_trans.inverted());

                Oasis::OasisBox box(rint(dbu_box_f.left()),
                                    rint(dbu_box_f.bottom()),
                                    rint(dbu_box_f.right()),
                                    rint(dbu_box_f.top()));

                Oasis::OasisTrans trans = vp_trans * dbu_trans;

                render::LayerMetaData l = (*it)->metadata();
                Oasis::LDType ld(l.get_layer_num(), l.get_data_type());
                render::LayoutView lv = m_layout_views[(*it)->view_index()];

//                render::RenderWorker* worker = new render::RenderWorker(lv.get_layout(), -1, box, vp_trans, ld, m_buffer[0]->width(), m_buffer[0]->height(), m_buffer[0]->resolution(), m_buffer[i * planes_per_layer], m_buffer[i * planes_per_layer + 1], m_buffer[i * planes_per_layer + 2]);
                render::RenderWorker* worker = new render::RenderWorker(lv.get_layout(), -1, box, trans, ld, m_buffer[0]->width(), m_buffer[0]->height(), m_buffer[0]->resolution(), m_buffer[i * planes_per_layer], m_buffer[i * planes_per_layer + 1], m_buffer[i * planes_per_layer + 2]);

                pool.start(worker);
            }
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
            m_image->fill(m_background_color);
        }

        set_plane_width(width());
        set_plane_height(height());
        set_plane_resolution(1.0);
        start_render();
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
    Oasis::OasisPointF p = m_vp.trans().inverted().transF(Oasis::OasisPointF((Oasis::float64)e->pos().x(), (Oasis::float64)m_vp.height() - 1 - e->pos().y()));
    Oasis::OasisBoxF vp_box = m_vp.box();
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
    
    zoom_box(Oasis::OasisBoxF((p.x() - ((p.x() - vp_box.left()) * f)),
                              (p.y() - ((p.y() - vp_box.bottom()) * f)),
                              (p.x() - ((p.x() - vp_box.right()) * f)),
                              (p.y() - ((p.y() - vp_box.top()) * f))
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
    Oasis::float64 x= p1.x();
    Oasis::float64 y = m_vp.height() - 1 - p1.y();
    Oasis::OasisPointF physics_p = m_vp.trans().inverted().transF(Oasis::OasisPointF(x, y));
    emit signal_pos_updated(physics_p.x(), physics_p.y());
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
    m_vp.set_box((Oasis::OasisBoxF(0.0, 0.0, 10.0, 10.0)));
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


Oasis::OasisBoxF RenderFrame::get_box() const
{
    return m_vp.box();
}


void RenderFrame::zoom_box(const Oasis::OasisBoxF& box)
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
    std::pair<bool, std::pair<QPoint, QPointF> > result = get_snap_point(pos);
    emit signal_get_snap_pos(result.first, result.second.first, result.second.second, mode);
}

void RenderFrame::slot_repaint_snap_ruler(QList<QPair<QPointF,QPointF> >lines)
{
    QList<QPair<QPointF, QPointF> > result;
    for(int i = 0; i < lines.size(); i++)
    {
        QPointF start = lines.at(i).first;
        QPointF end = lines.at(i).second;
        Oasis::OasisPointF pixel_p = m_vp.trans().transF(Oasis::OasisPointF(start.x(), start.y()));
        QPointF p1_new(pixel_p.x(), m_vp.height() - 1 - pixel_p.y());
        pixel_p = m_vp.trans().transF(Oasis::OasisPointF(end.x(), end.y()));
        QPointF p2_new(pixel_p.x(), m_vp.height() - 1 - pixel_p.y());
        result.append(QPair<QPointF,QPointF>(p1_new, p2_new));
    }
    emit signal_repaint_snap_ruler(result);
}

void RenderFrame::shift_view(Oasis::float64 scale, Oasis::float64 dx, Oasis::float64 dy)
{
    Oasis::OasisBoxF box = m_vp.box();
    Oasis::OasisPointF center((box.right() - box.left()) / 2 + box.left(), (box.top() - box.bottom()) / 2 + box.bottom());

    Oasis::OasisPointF new_center = center + Oasis::OasisPointF(dx * box.width(), dy * box.height());

    Oasis::float64 width = box.width() * scale;
    Oasis::float64 height = box.height() * scale;

    Oasis::OasisPointF p1(new_center.x() - 0.5 * width, new_center.y() - 0.5 * height);
    Oasis::OasisPointF p2(new_center.x() + 0.5 * width, new_center.y() + 0.5 * height);
    zoom_box(Oasis::OasisBoxF(p1.x(), p1.y(), p2.x(), p2.y()));

}

static Oasis::OasisBoxF calculate_box(double x, double y, const Oasis::OasisBoxF& box)
{
    Oasis::OasisPointF center(x,y);
    Oasis::float64 half_width = box.width() * 0.5;
    Oasis::float64 half_height = box.height() * 0.5;
    Oasis::OasisBoxF new_box(center.x() - half_width, center.y() - half_height, center.x() + half_width, center.y() + half_height);
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
    Oasis::OasisBoxF box = m_vp.box();
    zoom_box(calculate_box(x, y, box));
}


void RenderFrame::zoom_center(double x, double y)
{
    if( x < width() && x > 0 && y < height() && y > 0)
    {
        Oasis::OasisPointF point = m_vp.trans().inverted().transF(Oasis::OasisPointF(x, m_vp.height() - 1 - y));
        center_at_point(point.x(), point.y());
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
        layout = new Oasis::OasisLayout(prep_file_name);
        Oasis::OasisParser parser;
        parser.ImportFile(layout);
        lv.set_layout(layout);
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
    Oasis::float64 dbu = layout->get_dbu();

    Oasis::OasisBoxF new_box(box.left() * dbu,
                             box.bottom() * dbu,
                             box.right() * dbu,
                             box.top() * dbu);
    m_vp.set_size(width(), height());
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
    m_layout_views[lv_index].set_index(lv_index);
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
    Oasis::OasisBoxF physical_box(m_vp.box());
    emit signal_box_updated(physical_box.left(),
                            physical_box.bottom(),
                            physical_box.right(),
                            physical_box.top());
}

std::pair<bool, std::pair<QPoint, QPointF> >RenderFrame::get_snap_point(QPoint p1)
{
    Oasis::OasisPointF p = get_trans().inverted().transF(Oasis::OasisPointF(p1.x(), m_vp.height() - 1- p1.y()));
    Oasis::float64 snap_range = 0.01;
//    int snap_range = 10;
    std::pair<bool,Oasis::OasisPointF> result = snap_point(this, p, snap_range);
    if(result.first)
    {
        Oasis::OasisPointF snap_micron_p = result.second;
        Oasis::OasisPointF snap_bitmap_p = get_trans().transF(snap_micron_p);
        Oasis::OasisPoint snap_bitmap_p_int(snap_bitmap_p);

        QPoint snap_pix_q(snap_bitmap_p_int.x(), m_vp.height() - 1 - snap_bitmap_p_int.y());
        QPointF snap_micron_q(snap_micron_p.x(), snap_micron_p.y());
        return std::make_pair(true, std::make_pair(snap_pix_q, snap_micron_q));
    }
    else{
        return std::make_pair(false, std::make_pair(QPoint(), QPointF()));
    }
}

void RenderFrame::slot_refresh()
{
    set_view_ops();
    update();
}

void RenderFrame::slot_zoom_fit()
{
    Oasis::OasisBoxF result;
    for(size_t i = 0; i < m_layout_views.size(); i++)
    {
        Oasis::OasisLayout * layout = m_layout_views[i].get_layout();
        Oasis::OasisBox box = layout->get_bbox();
        Oasis::float64 dbu = layout->get_dbu();
        Oasis::OasisBoxF box_f(box.left() * dbu,
                               box.bottom() * dbu,
                               box.right() * dbu,
                               box.top() * dbu);
        result.update(box_f);
    }
    zoom_box(result);
}

}
