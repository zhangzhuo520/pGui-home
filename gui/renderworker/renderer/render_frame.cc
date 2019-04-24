#include "render_frame.h"

#include <QtGui>
#include <QPoint>
#include <QPointF>
#include <QLine>
#include <QLineF>
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
#include <QTime>

#include "render_bitmap.h"
#include "render_worker.h"
#include "render_layer_properties.h"
#include "render_bitmaps_to_image.h"
#include "render_bitmap_manager.h"
#include "render_palette.h"
#include "render_snap.h"
#include "render_defect_point.h"
#include "render_image.h"
#include "render_monitor_thread.h"

#include "oasis_parser.h"
#include "oasis_layout.h"
#include "oasis_exception.h"
#include "layout_preprocess.h"

#include "../../qt_logger/pgui_log_global.h"
#include "render_global.h"

namespace render{

static const oasis::float64 m_shift_unit = 0.1;

RenderFrame::RenderFrame(QWidget *parent):
    RenderObjectWidget(parent),
    m_image(0),
    m_image_bg(0),
    m_pixmap(0),
    m_resolution(1.0),
    m_redraw_required(false),
    m_update_image(false),
    m_bg_color(0x00ffffff),
    m_bitmap_manager(0),
    m_render_manager(0),
    m_restart_render(false),
    m_worker_nums(0)
{
    setBackgroundRole(QPalette::NoRole);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setMouseTracking(true);
    connect(this, SIGNAL(signal_down_key_pressed()), this, SLOT(slot_down_shift()));
    connect(this, SIGNAL(signal_up_key_pressed()), this, SLOT(slot_up_shift()));
    connect(this, SIGNAL(signal_left_key_pressed()), this, SLOT(slot_left_shift()));
    connect(this, SIGNAL(signal_right_key_pressed()), this, SLOT(slot_right_shift()));
    connect(this, SIGNAL(signal_zoom_out_pressed()), this, SLOT(slot_zoom_out()));
    connect(this, SIGNAL(signal_zoom_in_pressed()), this, SLOT(slot_zoom_in()));

    init_viewport();

    setFocusPolicy(Qt::StrongFocus);

    m_bitmap_manager = new BitmapManager();

    m_render_manager = new RenderManager();
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

    for(size_t i = 0; i < m_buffer.size(); i++)
    {
        delete m_buffer[i];
        m_buffer[i] = 0;
    }

    for(size_t i = 0; i < m_split_bitmaps.size(); i++)
    {
        delete m_split_bitmaps[i];
        m_split_bitmaps[i] = 0;
    }
    for(size_t i = 0; i < layers_size(); i++)
    {
        delete_layer(i);
    }

    for(int i = 0; i < layout_views_size(); i++)
    {
        std::vector<LayoutViewProxy>& proxys_vec = m_layout_views[i]->m_proxys;
        for(size_t j  = 0; j < proxys_vec.size(); j++)
        {
            if(proxys_vec[j].frame() == this)
            {
                LayoutView* lv = proxys_vec[j].m_layout_view;
                lv->erase_proxy(proxys_vec[j]);
            }
        }
    }
    delete m_bitmap_manager;
    delete m_render_manager;
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
    std::vector<render::ViewOp>().swap(m_view_ops);
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

void RenderFrame::prepare_drawing()
{
    if(m_redraw_required)
    {
        m_redraw_required = false;
        m_update_image = true;

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

        while(! m_split_bitmaps.empty())
        {
            if(m_split_bitmaps.back() != 0)
            {
                delete m_split_bitmaps.back();
            }
            m_split_bitmaps.pop_back();
        }

        std::vector<render::Bitmap*>().swap(m_buffer);
        std::vector<render::Bitmap*>().swap(m_split_bitmaps);

        m_draw_timer.start();
        if(m_image)
        {
            m_image->fill(m_bg_color);
        }
        if(m_restart_render)
        {
            m_render_manager->restart(this, m_required_redraw_layer);
        }
        else
        {
            set_worker_nums(50);
            m_render_manager->start(worker_nums(), this, m_vp, 1.0, false);
        }

        QString debug_info("first init finish");
        logger_file(debug_info);
        set_plane_width(width());
        set_plane_height(height());
        set_plane_resolution(1.0);

        if(m_render_manager->busy())
        {
            set_mouse_style(RenderFrame::Busy);
        }

    }
}


void RenderFrame::paintEvent(QPaintEvent * e )
{
    QString debug_info("ready to call paint event");
    logger_file(debug_info);
    prepare_drawing();
    if(m_image)
    {
        if(m_update_image || require_update_background())
        {
            m_update_image = false;
            if(require_update_background() || !m_image_bg)
            {
                debug_info = QString("ready ro repaint the background");
                logger_file(debug_info);
                m_image->fill(m_bg_color);
                render_background(m_vp, this);
                if(m_image_bg)
                {
                    delete m_image_bg;
                    m_image_bg = 0;
                }
                m_image_bg = new QImage(*m_image);
            }
            else
            {
                *m_image = *m_image_bg;
            }

            debug_info = QString("ready to merge to image");
            logger_file(debug_info)
            m_bitmap_manager->merge_to_image (m_view_ops, m_pattern, m_line_style, m_image, width(), height());

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

    e->ignore();
    debug_info = QString("call paintEvent finished");
    logger_file(debug_info);
}


void RenderFrame::wheelEvent(QWheelEvent * e)
{
    oasis::PointF p = m_vp.trans().inverted().transF(oasis::PointF((oasis::float64)e->pos().x(), (oasis::float64)m_vp.height() - 1 - e->pos().y()));
    oasis::BoxF vp_box = m_vp.box();
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
    

    oasis::BoxF fbox((p.x() - ((p.x() - vp_box.left()) * f)),
                     (p.y() - ((p.y() - vp_box.bottom()) * f)),
                     (p.x() - ((p.x() - vp_box.right()) * f)),
                     (p.y() - ((p.y() - vp_box.top()) * f)));

    QString debug_info("zoom_box in wheel event is called");
    logger_file(debug_info);
    zoom_box(fbox);
    debug_info = QString("zoom box in wheel event is completed.");
    logger_file(debug_info);
    e->ignore();
}


void RenderFrame::keyPressEvent(QKeyEvent * e)
{
    int key = e->key();
    if(key == Qt::Key_Down || key == Qt::Key_S)
    {
        emit signal_down_key_pressed();
    }
    else if(key  == Qt::Key_Up || key == Qt::Key_W)
    {
        emit signal_up_key_pressed();
    }
    else if(key == Qt::Key_Left || key == Qt::Key_A)
    {
        emit signal_left_key_pressed();
    }
    else if(key == Qt::Key_Right || key == Qt::Key_D)
    {
        emit signal_right_key_pressed();
    }
    else if((key == Qt::Key_Enter || key == Qt::Key_Return) && e->modifiers() == Qt::ShiftModifier)
    {
        emit signal_zoom_out_pressed();
    }
    else if(key == Qt::Key_Enter || key == Qt::Key_Return)
    {
        emit signal_zoom_in_pressed();
    }

    e->accept();
}

void RenderFrame::mouseMoveEvent(QMouseEvent* e)
{
    QPoint p1 = e->pos();
    oasis::float64 x= p1.x();
    oasis::float64 y = m_vp.height() - 1 - p1.y();
    oasis::PointF physics_p = m_vp.trans().inverted().transF(oasis::PointF(x, y));
    emit signal_pos_updated(physics_p.x(), physics_p.y());
    QWidget::mouseMoveEvent(e);
    e->ignore();
}

void RenderFrame::resizeEvent(QResizeEvent * e)
{
    QString debug_info("resizeEvent is called");
    logger_file(debug_info);
    m_vp.set_size(e->size().width(), e->size().height());
    redraw_all();
    slot_box_updated();
    QWidget::resizeEvent(e);
    e->ignore();
    debug_info = QString("resizeEvent is completed");
    logger_file(debug_info);
}

void RenderFrame::init_viewport()
{
    m_vp.set_size(100, 100);
    m_vp.set_box((oasis::BoxF(0.0, 0.0, 10.0, 10.0)));
}


const render::LayerProperties* RenderFrame::get_properties(int index) const
{
    return m_layers_properties[index];
}

void RenderFrame::set_layer_cached(int index, bool cache)
{
    if((size_t)index >= layers_size())
    {
        return ;
    }
    m_layers_properties[index]->set_cached(cache);
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

    set_view_ops();
    if(m_layers_properties[index]->cached())
    {
        update_image();
    }
    else
    {
        m_redraw_required = true;
        update_background();
        update();
    }
}


oasis::BoxF RenderFrame::get_box() const
{
    return m_vp.box();
}


void RenderFrame::zoom_box(const oasis::BoxF& box)
{
    QString debug_info("zoom box is called");
    logger_file(debug_info);
    m_vp.set_box(box);
    slot_box_updated();
    redraw_all();
    debug_info = QString("zoom box is completed");
    logger_file(debug_info);
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

void RenderFrame::slot_get_snap_pos(QPointF pos, int mode)
{
    int snap_mode = 0;

    if(mode == 1 || mode == 2)
    {
        snap_mode = 0;
    }
    else if(mode == 3)
    {
        snap_mode = 1;
    }
    else if(mode == 4)
    {
        snap_mode = 2;
    }
    else if(mode == 5)
    {
        snap_mode = 3;
    }
    else if(mode == 6)
    {
        snap_mode = 4;
    }

    std::pair<bool, std::pair<QPointF, QPointF> > result = get_snap_point(pos, snap_mode);
    emit signal_get_snap_pos(result.first, result.second.first, result.second.second, mode);
}

void RenderFrame::slot_get_snap_edge(QPointF pos, int mode)
{
    int snap_mode = 0;

    if(mode == 1 || mode == 2)
    {
        snap_mode = 0;
    }
    std::pair<bool, std::pair<QLineF, QLineF> > result = get_snap_edge(pos, snap_mode);
    emit signal_get_snap_edge(result.first, result.second.first, result.second.second, mode);
}

void RenderFrame::shift_view(oasis::float64 scale, oasis::float64 dx, oasis::float64 dy)
{
    for(RenderObjectWidget::object_iter it = begin_object(); it != end_object(); it++)
    {
        render::DefectPoint* point =  dynamic_cast<DefectPoint*> (*it);
        if(point->point_name() == "PosPoint")
        {
            erase_object(it);
            break;
        }
    }

    oasis::BoxF box = m_vp.box();
    oasis::PointF center((box.right() - box.left()) / 2 + box.left(), (box.top() - box.bottom()) / 2 + box.bottom());

    oasis::PointF new_center = center + oasis::PointF(dx * box.width(), dy * box.height());

    oasis::float64 width = box.width() * scale;
    oasis::float64 height = box.height() * scale;

    oasis::PointF p1(new_center.x() - 0.5 * width, new_center.y() - 0.5 * height);
    oasis::PointF p2(new_center.x() + 0.5 * width, new_center.y() + 0.5 * height);
    zoom_box(oasis::BoxF(p1.x(), p1.y(), p2.x(), p2.y()));
}

static oasis::BoxF calculate_box(double x, double y, const oasis::BoxF& box)
{
    oasis::PointF center(x,y);
    oasis::float64 half_width = box.width() * 0.5;
    oasis::float64 half_height = box.height() * 0.5;
    oasis::BoxF new_box(center.x() - half_width, center.y() - half_height, center.x() + half_width, center.y() + half_height);
    return new_box;
}

void RenderFrame::set_defect_point(double x, double y)
{
    QString debug_info("set defect point is called");
    logger_file(debug_info);
    for(RenderObjectWidget::object_iter it = begin_object(); it != end_object(); it++)
    {
        render::DefectPoint* point =  dynamic_cast<DefectPoint*> (*it);
        if(point != 0)
        {
            point->set_point(x, y);
            center_at_point(x, y);
            return;
        }
    }
    DefectPoint* point = new DefectPoint(x, y, this, true);
    point->set_point(x, y);

    center_at_point(x, y);
    debug_info =  QString("set_defect_point is completed");
    logger_file(debug_info)
}

void RenderFrame::center_at_point(double x, double y)
{
    for(RenderObjectWidget::object_iter it = begin_object(); it != end_object(); it++)
    {
        render::DefectPoint* point =  dynamic_cast<DefectPoint*> (*it);
        if(point->point_name() == "PosPoint")
        {
            erase_object(it);
            break;
        }
    }
    oasis::BoxF box = m_vp.box();
    zoom_box(calculate_box(x, y, box));
}

void RenderFrame::set_center_point(double x, double y, double view_range)
{
    bool is_exit = false;
    for(RenderObjectWidget::object_iter it = begin_object(); it != end_object(); it++)
    {
        render::DefectPoint* point =  dynamic_cast<DefectPoint*> (*it);
        if(point->point_name() == "PosPoint")
        {
            point->set_point(x, y);
            is_exit = true;
            break;
        }
    }
    if (!is_exit)
    {
        DefectPoint* point = new DefectPoint(x, y, this, true);
        point->set_point_name("PosPoint");
        point->set_point(x, y);
    }
    oasis::float64 left =  x - view_range / 2;
    oasis::float64 right = x + view_range / 2;
    oasis::float64 top = y + view_range / 2;
    oasis::float64 bottom = y - view_range / 2;
    oasis::BoxF box(left, bottom, right , top);

    zoom_box(box);
}


void RenderFrame::zoom_center(double x, double y)
{
    if( x < width() && x > 0 && y < height() && y > 0)
    {
        oasis::PointF point = m_vp.trans().inverted().transF(oasis::PointF(x, m_vp.height() - 1 - y));
        center_at_point(point.x(), point.y());
    }
}

void RenderFrame::detach_layout_view(LayoutView* lv)
{
    int index = index_of_layout_views(lv);
    erase_layout_view(index);
}

void RenderFrame::load_layout_view(render::LayoutView* lv, std::string prep_dir, bool add_layout_view)
{
    QTime t;
    t.start();

    std::string file_name = lv->file_name();
    oasis::OasisLayout* layout = new oasis::OasisLayout(file_name);

    lv->set_layout(layout);
    oasis::OasisDebug::loglevel(-1);
    qDebug() << "set oasis debug level -1";
    try
    {
        oasis::OasisParser parser;
        parser.import_file(layout);
    }
    catch (oasis::OasisException& e)
    {
        oasis::prep_options option;
        option.thread_count = 20;
        std::string prep_file_name = oasis::preprocessLayout(file_name, prep_dir, false, option);
        layout = new oasis::OasisLayout(prep_file_name);
        oasis::OasisParser parser;
        parser.import_file(layout);
        lv->set_layout(layout);
    }

    QString info = QString("preprocess: %1 use time: %2 ms ").arg(file_name.c_str()).arg(t.elapsed());
    ui::logger_widget(info);
    logger_file(info);

    if(!add_layout_view)
    {
        clear_layout_view();
    }

    int lv_index = layout_views_size();
    set_layout_view(lv, lv_index);

    std::set<std::pair<int, int> >layers;
    layout->get_layers(layers);

    create_and_initial_layer_properties(lv_index, layers);

    double left = std::numeric_limits<double>::max();
    double bottom = left;
    double right = std::numeric_limits<double>::min();
    double top = right;

    if(add_layout_view)
    {
        m_vp.set_size(width(), height());
    }
    else
    {
        render::LayoutView* tmp = get_layout_view(0);
        oasis::OasisLayout* layout = tmp->get_layout();
        oasis::Box bound = layout->get_bbox();
        oasis::float64 dbu = layout->get_dbu();
        left = bound.left() * dbu< left ? bound.left() * dbu: left;
        bottom = bound.bottom() * dbu < bottom ? bound.bottom() * dbu : bottom;
        right = bound.right() * dbu > right ? bound.right() * dbu: right;
        top = bound.top() * dbu > top ? bound.top() * dbu : top;

        oasis::BoxF box(left, bottom, right, top);
        m_vp.set_size(width(), height());
        m_vp.set_box(box);
    }
    update_view_ops();
    emit signal_layout_view_changed(this);
}

void  RenderFrame::add_layout_view(render::LayoutView* lv, bool add_layout_view)
{
    if(!add_layout_view)
    {
        clear_layout_view();
    }

    int lv_index = layout_views_size();
    set_layout_view(lv, lv_index);

    std::set<std::pair<int, int> >layers;
    oasis::OasisLayout* layout = lv->get_layout();
    layout->get_layers(layers);

    create_and_initial_layer_properties(lv_index, layers);

    if(add_layout_view)
    {
        m_vp.set_size(width(), height());
    }
    else
    {
        double left = std::numeric_limits<double>::max();
        double bottom = left;
        double right = std::numeric_limits<double>::min();
        double top = right;

        render::LayoutView* tmp = get_layout_view(0);
        oasis::OasisLayout* layout = tmp->get_layout();
        oasis::Box bound = layout->get_bbox();
        oasis::float64 dbu = layout->get_dbu();
        left = bound.left() * dbu< left ? bound.left() * dbu: left;
        bottom = bound.bottom() * dbu < bottom ? bound.bottom() * dbu : bottom;
        right = bound.right() * dbu > right ? bound.right() * dbu: right;
        top = bound.top() * dbu > top ? bound.top() * dbu : top;

        oasis::BoxF box(left, bottom, right, top);
        m_vp.set_size(width(), height());
        m_vp.set_box(box);
    }

    update_view_ops();
    emit signal_layout_view_changed(this);
}

LayoutView* RenderFrame::get_layout_view(int index)
{
    if(index > 0 && index < layout_views_size())
    {
        return m_layout_views[index].m_layout_view;
    }
    else
    {
        return m_layout_views[0].m_layout_view;
    }
}

void RenderFrame::set_layout_view(render::LayoutView* lv, int lv_index)
{
    while(lv_index >= layout_views_size())
    {
        m_layout_views.push_back(LayoutViewProxy());
    }
    LayoutViewProxy proxy(lv, this);
    lv->m_proxys.push_back(proxy);
    m_layout_views[lv_index] = proxy;
}

void RenderFrame::create_and_initial_layer_properties(int lv_index, std::set<std::pair<int,int> >& layers)
{
    int i = 0;
    for(std::set<std::pair<int,int> >::iterator it = layers.begin(); it != layers.end(); it++, i++)
    {
        int layer_num = it->first;
        int data_type = it->second;
        oasis::OasisLayout* layout = m_layout_views[lv_index]->get_layout();
        m_layers_properties.push_back(new render::LayerProperties());
        m_layers_properties.back()->set_view_index(lv_index);
        m_layers_properties.back()->set_view(this);
        m_layers_properties.back()->set_metadata(layout->get_layer_name(layer_num, data_type), layer_num, data_type);
        m_layers_properties.back()->set_layer_index(std::distance(layers.begin(), it));
        m_layers_properties.back()->set_fill_color(render::color_table[i]);
        m_layers_properties.back()->set_frame_color(render::color_table[i]);
        m_layers_properties.back()->set_pattern(1);
        m_layers_properties.back()->set_line_width(1);
        m_layers_properties.back()->set_line_style(0);
        m_layers_properties.back()->set_visible(true);
        if(i == render::color_table_size - 1)
        {
            i = -1;
        }
    }
}

// force to redraw
void RenderFrame::update_view_ops()
{
    set_view_ops();
    m_redraw_required = true;
}


void RenderFrame::erase_layout_view(int index)
{
    if( index < 0 || (size_t)index >= m_layout_views.size())
    {
        return ;
    }

    std::vector<render::LayoutViewProxy>::iterator it = m_layout_views.begin() + index;
    render::LayoutViewProxy proxy= *it;
    render::LayoutView* lv = proxy.m_layout_view;
    lv->erase_proxy(proxy);
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

    update_view_ops();
    update();
    emit signal_layout_view_changed(this);
}

void RenderFrame::clear_layout_view()
{
    while(layers_size() > 0)
    {
        delete_layer(layers_size() - 1);
    }
    m_layout_views.clear();
    std::vector<render::LayoutViewProxy>().swap(m_layout_views);

}

void RenderFrame::delete_layer(int index)
{
    delete m_layers_properties[index];
    m_layers_properties.erase(m_layers_properties.begin() + index);
}

void RenderFrame::slot_box_updated()
{
    oasis::BoxF physical_box(m_vp.box());
    emit signal_box_updated(physical_box.left(),
                            physical_box.bottom(),
                            physical_box.right(),
                            physical_box.top());
}

std::pair<bool, std::pair<QPointF, QPointF> >RenderFrame::get_snap_point(QPointF p1, int mode)
{
    oasis::PointF p = get_trans().inverted().transF(oasis::PointF(p1.x(), m_vp.height() - 1- p1.y()));

    int snap_pixel = 10;
    oasis::float64 snap_range = get_trans().inverted().mag() * snap_pixel;
    std::pair<bool,oasis::PointF> result = snap_point(this, p, snap_range, mode);
    if(result.first)
    {
        oasis::PointF snap_micron_p = result.second;
        oasis::PointF snap_bitmap_p = get_trans().transF(snap_micron_p);

        QPointF snap_pix_q(snap_bitmap_p.x(), m_vp.height() - 1 - snap_bitmap_p.y());
        QPointF snap_micron_q(snap_micron_p.x(), snap_micron_p.y());
        return std::make_pair(true, std::make_pair(snap_pix_q, snap_micron_q));
    }
    else{
        return std::make_pair(false, std::make_pair(p1, QPointF(p.x(), p.y())));
    }
}

std::pair<bool, std::pair<QLineF, QLineF> > RenderFrame::get_snap_edge(QPointF p1, int mode)
{
    oasis::PointF p = get_trans().inverted().transF(oasis::PointF(p1.x(), m_vp.height() - 1 - p1.y()));
    int snap_pixel = 10;
    oasis::float64 snap_range = get_trans().inverted().mag() * snap_pixel;

    std::pair<bool, oasis::EdgeF> result = snap_edge(this, p, snap_range, mode);
    if(result.first)
    {
        oasis::EdgeF snap_micron_edge = result.second;
        oasis::EdgeF snap_bitmap_edge(get_trans().transF(snap_micron_edge.p1()), get_trans().transF(snap_micron_edge.p2()));
        QLineF result_pix(QPointF(snap_bitmap_edge.p1().x(), m_vp.height() - 1 - snap_bitmap_edge.p1().y()),
                          QPointF(snap_bitmap_edge.p2().x(), m_vp.height() - 1 - snap_bitmap_edge.p2().y()));
        QLineF result_micron(QPointF(snap_micron_edge.p1().x(), snap_micron_edge.p1().y()),
                             QPointF(snap_micron_edge.p2().x(), snap_micron_edge.p2().y()));
        return std::make_pair(true, std::make_pair(result_pix, result_micron));
    }
    else
    {
        return std::make_pair(false, std::make_pair(QLineF(), QLineF()));
    }

}

void RenderFrame::slot_refresh()
{
    set_view_ops();
    update_image();
}

void RenderFrame::slot_zoom_fit()
{
    oasis::BoxF result;
    for(size_t i = 0; i < m_layout_views.size(); i++)
    {
        oasis::OasisLayout * layout = m_layout_views[i]->get_layout();
        oasis::Box box = layout->get_bbox();
        oasis::float64 dbu = layout->get_dbu();
        oasis::BoxF box_f(box.left() * dbu,
                               box.bottom() * dbu,
                               box.right() * dbu,
                               box.top() * dbu);
        result.update(box_f);
    }
    zoom_box(result);
}

int RenderFrame::index_of_layout_views(LayoutView* lv)
{
    int result = 0;
    for (std::vector<LayoutViewProxy>::iterator it = m_layout_views.begin(); it != m_layout_views.end(); it++, result++)
    {
        if((*it) ==  lv)
        {
            return result;
        }
    }
    return -1;
}

int RenderFrame::tree_to_list_index(int parent_index, int child_index)
{
    if(parent_index == -1 || child_index == -1)
    {
        return -1;
    }

    int result = 0;
    for(int i = 0; i < parent_index; i++)
    {
        LayoutView* lv = get_layout_view(i);
        std::set<std::pair<int, int> >layers;
        oasis::OasisLayout* layout = lv->get_layout();
        layout->get_layers(layers);
        int count = layers.size();
        result += count;
    }
    result += child_index;
    return result;
}

void RenderFrame::list_to_tree_index(int index, int &parent_index, int& child_index)
{
    const LayerProperties* lp = get_properties(index);
    parent_index = lp->view_index();
    for(size_t i = 0; i < layers_size(); i++)
    {
        if(get_properties(i)->view_index() == parent_index)
        {
            child_index = index - i;
            return ;
        }
    }
}

double RenderFrame::get_view_range() const
{

    oasis::BoxF box = m_vp.box();
    return std::min(box.width(), box.height());
}

std::vector<render::LayoutView*> RenderFrame::get_layout_views_list() const
{
    std::vector<render::LayoutView*> result;
    for(size_t i = 0; i < m_layout_views.size(); i++)
    {
        result.push_back(m_layout_views[i].m_layout_view);
    }

    return result;
}

void RenderFrame::update_image()
{
    m_update_image = true;
    update();
}

void RenderFrame::init_bitmaps(unsigned int layers,
                               unsigned int width,
                               unsigned int height,
                               double resolution,
                               bool shift,
                               const oasis::Point &shift_disp,
                               bool restart,
                               const std::vector<int> &restart_bitmaps)
{
    if(m_bitmap_manager)
    {
        m_bitmap_manager->init_bitmaps(layers, width, height, resolution,shift, shift_disp, restart, restart_bitmaps);
    }
}

void RenderFrame::finish_drawing()
{
    set_mouse_style(RenderFrame::Normal);
    QString info = QString("finish once draw time, use time: %1 ms").arg(m_draw_timer.elapsed());
    ui::logger_widget(info);
    info = QString("finish once draw time, use time: %1 ms").arg(m_draw_timer.elapsed());
    logger_file(info);
}

Bitmap* RenderFrame::create_bitmap()
{
    if(m_bitmap_manager)
    {
        return m_bitmap_manager->create_bitmap();
    }
    else
    {
        return static_cast<Bitmap*>(0);
    }
}

void RenderFrame::set_mouse_style(MouseStyle style)
{
    switch(style)
    {
        case Normal:
        {
            setCursor(Qt::ArrowCursor);
            break;
        }
        case Busy:
        {
            setCursor(Qt::WaitCursor);
            break;
        }
        default:
        {
            setCursor(Qt::ArrowCursor);
        }
    }
}


void RenderFrame::set_background_color(QColor color)
{
    m_bg_color = color.rgb();
    update_image();
}


void RenderFrame::add_rts_image(const QStringList& info_path_list)
{
    for(int i = 0; i < info_path_list.count(); i++)
    {
        QFile file(info_path_list[i]);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            break;
        }
        QTextStream text(&file);
        QString line_data;
        bool parse_result = true;
        int line_num = 0;
        int width = 0;
        int height = 0;
        int pixel_space = 0;
        long left = 0;
        long bottom = 0;
        std::string image_path;
        while(!text.atEnd())
        {
            line_data = text.readLine();
            line_data.trimmed();
            switch(line_num++)
            {
                case 0:
                    break;
                case 1:
                {
                    QString split_char = "=";
                    int index = line_data.lastIndexOf(split_char);
                    if(index == -1)
                    {
                        parse_result = false;
                    }
                    else
                    {
                        QString str_width = line_data.right(line_data.size() - 1 - index);
                        width = str_width.toInt();
                    }
                    break;
                }

                case 2:
                {
                    QString split_char = "=";
                    int index = line_data.lastIndexOf(split_char);
                    if(index == -1)
                    {
                        parse_result = false;
                    }
                    else
                    {
                        QString str_height = line_data.right(line_data.size() - 1 - index);
                        height = str_height.toInt();
                    }
                    break;
                }
                case 3:
                {
                    QString split_char = "=";
                    int index = line_data.lastIndexOf(split_char);
                    if(index == -1)
                    {
                        parse_result = false;
                    }
                    else
                    {
                       QString remain = line_data.right(line_data.size() - 1 - index);
                       QStringList pos_list = remain.split(",");
                       if(pos_list.size() != 3)
                       {
                           parse_result = false;
                       }
                       left = pos_list[0].toLong();
                       bottom = pos_list[1].toLong();
                       pixel_space = pos_list[2].toInt();

                    }
                    break;
                }
                case 4:
                {
                    QString split_char = "=";
                    int index = line_data.lastIndexOf(split_char);
                    if(index == -1)
                    {
                        parse_result = false;
                    }
                    else
                    {
                        image_path = line_data.right(line_data.size() - 1 - index).toStdString();
                    }
                    break;
                }
               default:
                    parse_result = false;
            }
            if(!parse_result)
            {
                break;
            }
        }
        file.close();

        if(parse_result)
        {
            LayoutView* lv = get_layout_view(0);
            double dbu = lv->get_dbu();
            RTSImage* image = new RTSImage(width, height, left, bottom, pixel_space, image_path, dbu, this);
            image->redraw();
        }
    }
}

void RenderFrame::set_worker_nums(int workers)
{
    m_worker_nums = std::max(0, std::min(50, workers));
}

void RenderFrame::copy_bitmaps(const std::vector<int>& update_layers, const std::vector<render::Bitmap*>& planes, int x, int y)
{
    if(m_bitmap_manager)
    {
        m_bitmap_manager->copy_bitmaps_with_shift(update_layers, planes, x, y);
    }
}

void RenderFrame::copy_bitmap(int update_layers, Bitmap* contour_bitmap, Bitmap* fill_bitmap, int x_offset, int y_offset)
{
    if(m_bitmap_manager)
    {
        m_bitmap_manager->copy_bitmap_with_shift(update_layers * 3 , contour_bitmap, x_offset, y_offset);
        m_bitmap_manager->copy_bitmap_with_shift(update_layers * 3 + 1, fill_bitmap, x_offset, y_offset);
    }
}

void RenderFrame::stop_redraw()
{
    m_render_manager->stop();
}

void RenderFrame::redraw_all()
{
    QString debug_info("RenderFrame::redraw_all begin");
    logger_file(debug_info)
    stop_redraw();
    debug_info = QString("RenderFrame::redraw_all end");
    logger_file(debug_info)
    if(!m_redraw_required)
    {
        m_required_redraw_layer.clear();
    }
    m_redraw_required = true;
    update_background();
    update();
}

std::vector<RTSImage*> RenderFrame::get_rts_images()
{
    std::vector<BackgroundObject*>& bg_objects = get_bg_objects();
    std::vector<RTSImage*> result;
    for(size_t i = 0; i < bg_objects.size(); i++)
    {
        RTSImage* image = dynamic_cast<RTSImage*>(bg_objects[i]);
        if(image)
        {
            result.push_back(image);
        }
    }
    return result;
}

std::vector<QString> RenderFrame::get_rts_image_paths()
{
    std::vector<RTSImage*> list = get_rts_images();
    std::vector<QString> result;
    for(size_t i = 0; i < list.size(); i++)
    {
        result.push_back(QString::fromStdString(list[i]->file_path()));
    }
    return result;
}


void RenderFrame::disable_rts_image(QString& file_path)
{
    std::vector<RTSImage* > images = get_rts_images();
    for(size_t i = 0; i < images.size(); i++)
    {
        if(images[i]->file_path() == file_path.toStdString())
        {
            images[i]->set_visible(false);
            break;
        }
    }
}

void RenderFrame::enable_rts_image(QString &file_path)
{
    std::vector<RTSImage* > images = get_rts_images();
    for(size_t i = 0; i < images.size(); i++)
    {
        if(images[i]->file_path() == file_path.toStdString())
        {
            images[i]->set_visible(true);
            break;
        }
    }
}

void RenderFrame::disble_all_rts_image()
{
    std::vector<RTSImage* > images = get_rts_images();
    for(size_t i = 0; i < images.size(); i++)
    {
        images[i]->set_visible(false);
    }
}

void RenderFrame::clear_all_rts_images()
{
    std::vector<BackgroundObject*>& bg_objects = get_bg_objects();
    for(int i = 0; i < (int)bg_objects.size();i++)
    {
        RTSImage* image = dynamic_cast<RTSImage*>(bg_objects[i]);
        if(image)
        {
            delete image;
            bg_objects[i] = 0;
            bg_objects.erase(bg_objects.begin() + i);
            i--;
        }
    }
}

void RenderFrame::slot_go_to_position(QPointF p)
{
    center_at_point(p.x(), p.y());
}

}
