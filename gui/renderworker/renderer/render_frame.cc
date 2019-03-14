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
#include <QTime>

#include "render_worker.h"
#include "render_bitmaps_to_image.h"
#include "render_palette.h"
#include "render_snap.h"
#include "render_defect_point.h"

#include "oasis_parser.h"
#include "oasis_layout.h"
#include "oasis_exception.h"
#include "layout_preprocess.h"

#include "../../qt_logger/pgui_log_global.h"

namespace render{

static const oasis::float64 m_shift_unit = 0.1;

RenderFrame::RenderFrame(QWidget *parent):
    RenderObjectWidget(parent),
    m_image(0),
    m_pixmap(0),
    m_resolution(1.0),
    m_background(0),
    m_redraw_required(false),
    m_update_image(false),
    m_window_max_size(120.0)
{
    setBackgroundRole(QPalette::NoRole);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_DeleteOnClose, true);
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

    for(size_t i = 0; i < m_buffer.size(); i++)
    {
        delete m_buffer[i];
        m_buffer[i] = 0;
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
    oasis::BoxF micron_box_f = m_vp.box();
    oasis::OasisTrans vp_trans = m_vp.trans();
    unsigned int i = 0;
    for (std::vector<render::LayerProperties*>::iterator it = m_layers_properties.begin(); it != m_layers_properties.end(); it++, i++)
    {
        if((*it)->visible())
        {
//            render::LayoutView* lv = m_layout_views[(*it)->view_index()];
            render::LayoutViewProxy lv = m_layout_views[(*it)->view_index()];
            oasis::float64 dbu = lv->get_dbu();

            oasis::OasisTrans dbu_trans(false, 0.0, dbu, oasis::PointF(0.0, 0.0));
            oasis::BoxF dbu_box_f = micron_box_f.transed(dbu_trans.inverted());

            oasis::Box box(rint(dbu_box_f.left()),
                                rint(dbu_box_f.bottom()),
                                rint(dbu_box_f.right()),
                                rint(dbu_box_f.top()));

            oasis::OasisTrans trans = vp_trans * dbu_trans;

            render::LayerMetaData l = (*it)->metadata();
            oasis::LDType ld(l.get_layer_num(), l.get_data_type());
            render::RenderWorker* worker = new render::RenderWorker(lv->get_layout(), -1, box, trans, ld, m_buffer[0]->width(), m_buffer[0]->height(), m_buffer[0]->resolution(), m_buffer[i * planes_per_layer], m_buffer[i * planes_per_layer + 1], m_buffer[i * planes_per_layer + 2]);

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
            m_image->fill(m_background_color);
        }

        set_plane_width(width());
        set_plane_height(height());
        set_plane_resolution(1.0);
        QTime t;
        t.start();
		setCursor(Qt::WaitCursor);
        start_render();
		setCursor(Qt::ArrowCursor);
        ui::logger_widget(QString("finish draw  use time: %1 ms").arg(t.elapsed()));
        logger_file(QString("finish draw  use time: %1 ms").arg(t.elapsed()));
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
    e->ignore();
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

    if(f > 1)
    {
        if(std::min(fbox.width(), fbox.height()) < m_window_max_size)
        {
            zoom_box(fbox);
        }
    }
    else
    {
        zoom_box(fbox);
    }

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
    e->ignore();
    m_redraw_required = true;
    m_vp.set_size(e->size().width(), e->size().height());
    slot_box_updated();
    QWidget::resizeEvent(e);
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


oasis::BoxF RenderFrame::get_box() const
{
    return m_vp.box();
}


void RenderFrame::zoom_box(const oasis::BoxF& box)
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


void RenderFrame::shift_view(oasis::float64 scale, oasis::float64 dx, oasis::float64 dy)
{
    oasis::BoxF box = m_vp.box();
    oasis::PointF center((box.right() - box.left()) / 2 + box.left(), (box.top() - box.bottom()) / 2 + box.bottom());

    oasis::PointF new_center = center + oasis::PointF(dx * box.width(), dy * box.height());

    oasis::float64 width = box.width() * scale;
    oasis::float64 height = box.height() * scale;

    oasis::PointF p1(new_center.x() - 0.5 * width, new_center.y() - 0.5 * height);
    oasis::PointF p2(new_center.x() + 0.5 * width, new_center.y() + 0.5 * height);

    if(scale >= 1.2)
    {
        if(std::min(width, height) < m_window_max_size)
        {
            zoom_box(oasis::BoxF(p1.x(), p1.y(), p2.x(), p2.y()));
        }
        else
        {
            qDebug() << "zooming out exceeds the maximum limitation.";
        }
    }
    else
    {
        zoom_box(oasis::BoxF(p1.x(), p1.y(), p2.x(), p2.y()));
    }

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
    oasis::BoxF box = m_vp.box();
    zoom_box(calculate_box(x, y, box));
}

void RenderFrame::set_center_point(double x, double y, double view_range)
{

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

    ui::logger_widget(QString("preprocess: %1 use time: %2 ms ").arg(file_name.c_str()).arg(t.elapsed()));
    logger_file(QString("preprocess: %1 use time: %2 ms ").arg(file_name.c_str()).arg(t.elapsed()));
    if(!add_layout_view)
    {
        clear_layout_view();
    }

    int lv_index = layout_views_size();
    set_layout_view(lv, lv_index);

    std::set<std::pair<int, int> >layers;
    layout->get_layers(layers);

    create_and_initial_layer_properties(lv_index, layers);

    if(!add_layout_view)
    {
        double left = std::numeric_limits<double>::max();
        double bottom = left;
        double right = std::numeric_limits<double>::min();
        double top = right;

        for(int i = 0; i < layout_views_size(); i++)
        {
            render::LayoutView* tmp = get_layout_view(i);
            oasis::OasisLayout* layout = tmp->get_layout();
            oasis::Box bound = layout->get_bbox();
            oasis::float64 dbu = layout->get_dbu();
            left = bound.left() * dbu< left ? bound.left() * dbu: left;
            bottom = bound.bottom() * dbu < bottom ? bound.bottom() * dbu : bottom;
            right = bound.right() * dbu > right ? bound.right() * dbu: right;
            top = bound.top() * dbu > top ? bound.top() * dbu : top;
        }

        oasis::BoxF box(left, bottom, right, top);

        oasis::float64 view_range = m_window_max_size / 2;
        oasis::PointF center(box.left() + box.width() / 2 , box.bottom() + box.height() / 2);

        oasis::BoxF new_box(center.x() - view_range,
                            center.y() - view_range,
                            center.x() + view_range,
                            center.y() + view_range);

        m_vp.set_size(width(), height());
        m_vp.set_box(new_box);
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

    if(!add_layout_view)
    {
        double left = std::numeric_limits<double>::max();
        double bottom = left;
        double right = std::numeric_limits<double>::min();
        double top = right;

        for(int i = 0; i < layout_views_size(); i++)
        {
            render::LayoutView* tmp = get_layout_view(i);
            oasis::OasisLayout* layout = tmp->get_layout();
            oasis::Box bound = layout->get_bbox();
            oasis::float64 dbu = layout->get_dbu();
            left = bound.left() * dbu< left ? bound.left() * dbu: left;
            bottom = bound.bottom() * dbu < bottom ? bound.bottom() * dbu : bottom;
            right = bound.right() * dbu > right ? bound.right() * dbu: right;
            top = bound.top() * dbu > top ? bound.top() * dbu : top;
        }

        oasis::BoxF box(left, bottom, right, top);

        oasis::float64 view_range = m_window_max_size / 2;
        oasis::PointF center(box.left() + box.width() / 2 , box.bottom() + box.height() / 2);

        oasis::BoxF new_box(center.x() - view_range,
                            center.y() - view_range,
                            center.x() + view_range,
                            center.y() + view_range);

        m_vp.set_size(width(), height());
        m_vp.set_box(new_box);
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
//    oasis::float64 snap_range = 0.01;

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

void RenderFrame::slot_refresh()
{
    set_view_ops();
    update();
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

void RenderFrame::set_window_max_size(double view_range)
{
    m_window_max_size = view_range;
    oasis::BoxF box = m_vp.box();
    oasis::PointF center(box.left() + box.width() / 2 , box.bottom() + box.height() / 2);

    oasis::BoxF new_box(center.x() - view_range / 2,
                        center.y() - view_range / 2,
                        center.x() + view_range / 2,
                        center.y() + view_range / 2);
    zoom_box(new_box);
}

int RenderFrame::index_of_layout_views(LayoutView* lv)
{
    int result = 0;
//    for (std::vector<LayoutView*>::iterator it = m_layout_views.begin(); it != m_layout_views.end(); it++, result++)
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

void RenderFrame::set_background_color(QColor color)
{
    int red = color.red();
    int green = color.green();
    int blue = color.blue();
    int sum_color =  red << 16 | green << 8 | blue;
    m_background_color = (unsigned int) sum_color;
    m_redraw_required = true;
    update();
}

}
