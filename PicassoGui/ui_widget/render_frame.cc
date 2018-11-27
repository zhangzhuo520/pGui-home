#include "render_frame.h"

#include <QtGui>
#include <QPoint>
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

#include "render_defect_point.h"

#include "OasisParser.h"
#include "OasisLayout.h"

namespace render{

static const Oasis::float64 m_shift_unit = 0.1;

RenderFrame::RenderFrame(QWidget *parent, QString path):
    RenderObjectWidget(parent),
    m_image(0),
    m_pixmap(0),
    m_resolution(1.0),
    m_background(0),
    m_layout(0),
    m_redraw_required(false),
    m_update_image(false)
{
    setBackgroundRole(QPalette::NoRole);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    load_file(path.toStdString());
    connect(this, SIGNAL(signal_down_key_pressed()), this, SLOT(slot_down_shift()));
    connect(this, SIGNAL(signal_up_key_pressed()), this, SLOT(slot_up_shift()));
    connect(this, SIGNAL(signal_left_key_pressed()), this, SLOT(slot_left_shift()));
    connect(this, SIGNAL(signal_right_key_pressed()), this, SLOT(slot_right_shift()));
    
	setFocusPolicy(Qt::StrongFocus);

    set_defect_point(0, 70);
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
        m_layers_properties.back().set_line_width(1);
        if(j == render::color_table_size - 1)
        {
            j = -1;
        }
    }

    set_view_ops();
    init_viewport();

    return m_layout;
}

void RenderFrame::set_view_ops()
{
//    bool bright = background_color().green() > 128;
    size_t layers = m_layers_properties.size();
    size_t planes_per_layer = 3;

    m_view_ops.clear();
    m_view_ops.reserve(layers * planes_per_layer);
    render::ViewOp::Mode mode = render::ViewOp::Copy;

    for(std::vector<render::LayerProperties>::iterator it = m_layers_properties.begin(); it != m_layers_properties.end(); it++)
    {
        //contour fill vertex
        if(it->visible())
        {
            m_view_ops.push_back(render::ViewOp(it->frame_color(), mode, it->line_style(), 0, render::ViewOp::Rect, it->line_width()));
            m_view_ops.push_back(render::ViewOp(it->fill_color(), mode, 0, it->pattern()));
            m_view_ops.push_back(render::ViewOp(it->fill_color(), mode, 0, it->pattern()));
        }
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

    QThreadPool pool;
    pool.setMaxThreadCount(layers);

    m_vp.set_size(width(), height());
    Oasis::OasisBox box = m_vp.box();
    Oasis::OasisTrans vp_trans = m_vp.trans();
    unsigned int i = 0;
    for (std::vector<render::LayerProperties>::const_iterator it = m_layers_properties.begin(); it != m_layers_properties.end(); it++, i++)
    {
        if(it->visible())
        {
            render::LayerMetaData l = it->metadata();
            Oasis::LDType ld(l.get_layer_num(), l.get_data_type());
//            render::RenderWorker worker(m_layout, -1, box, vp_trans, ld, m_buffer[0]->width(), m_buffer[0]->height(), m_buffer[0]->resolution(), m_buffer[i * planes_per_layer], m_buffer[i * planes_per_layer + 1], m_buffer[i * planes_per_layer + 2]);
//            worker.run();
          render::RenderWorker* worker = new render::RenderWorker(m_layout, -1, box, vp_trans, ld, m_buffer[0]->width(), m_buffer[0]->height(), m_buffer[0]->resolution(), m_buffer[i * planes_per_layer], m_buffer[i * planes_per_layer + 1], m_buffer[i * planes_per_layer + 2]);
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
            m_image->fill(palette().color(QPalette::Normal, QPalette::Background));
        }

        init();
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


const render::LayerProperties& RenderFrame::get_properties(int index) const
{
    return m_layers_properties[index];
}

const std::vector<render::LayerProperties>& RenderFrame::get_properties_list() const
{
    return m_layers_properties;
}


//void RenderFrame::set_properties(const LayerProperties& lp, int index)
void RenderFrame::set_properties(const LayerProperties& lp)
{
//    if(index >= layers_size())
//    {
//        return;
//    }
    int index = lp.layer_index();
    m_layers_properties[index] = lp;
//    lp.attach_view(this);
//    lp.set_layer_index(index);
    set_view_ops();
	m_redraw_required = true;
    update();
}


Oasis::OasisBox RenderFrame::get_box() const
{
    return m_vp.box();
}


void RenderFrame::zoom_box(const Oasis::OasisBox& box)
{
    m_vp.set_box(box);
    m_redraw_required = true;
    Oasis::OasisBox physical_box(m_vp.box());
    emit signal_box_updated((double)physical_box.left() / render::dbu,
                            (double)physical_box.bottom() /render::dbu,
                            (double)physical_box.right() / render::dbu,
                            (double)physical_box.top() / render::dbu);
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

void RenderFrame::slot_distance_point(QPointF start, QPointF end)
{
    qDebug() << "start :" << start << "end :" << end;
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
    Oasis::OasisPoint center(Oasis::int64(x * render::dbu), Oasis::int64(y * render::dbu));
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
    zoom_box(calculate_box(x, y, box));
}


void RenderFrame::zoom_center(int x, int y)
{
    if( x < width() && x > 0 && y < height() && y > 0)
    {
        Oasis::OasisPoint point = m_vp.trans().inverted().trans(Oasis::OasisPoint(x, m_vp.height() - 1 - y));
        center_at_point((double)point.x() / render::dbu, (double)point.y() / render::dbu );
    }
}

}
