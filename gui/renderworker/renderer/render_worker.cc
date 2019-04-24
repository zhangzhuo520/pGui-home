#ifndef RENDER_WORKER_CC
#define RENDER_WORKER_CC

#include "render_worker.h"
#include "render_frame.h"
#include "render_bitmap.h"
#include "render_layer_properties.h"
#include "render_layout_view.h"
#include "render_monitor_thread.h"

#include "../../qt_logger/pgui_log_global.h"
#include "render_cell_cache.h"
#include "render_hash.h"
#include "render_global.h"

#include <QDebug>

namespace render
{

static bool include(oasis::BoxF& box1, oasis::BoxF& box2)
{
    if(box2.left() < box1.left())
    {
        return false;
    }

    if(box2.right() > box1.right())
    {
        return false;
    }

    if(box2.bottom() < box1.bottom())
    {
        return false;
    }

    if(box2.top() > box1.top())
    {
        return false;
    }

    return true;
}

static int compress_trans(oasis::OasisTrans& t1)
{
    int result = 0;
    oasis::float64 angle = t1.angle();
    if(angle > 0 && angle <= 90)
    {
        result = 0;
    }
    else if(angle > 90 && angle <= 180)
    {
        result = 1;
    }
    else if(angle > 180 && angle <= 270)
    {
        result = 2;
    }
    else
    {
        result = 3;
    }
    return result + t1.mag() ? 4 : 0;

}


static std::vector<oasis::BoxF> diff_box(const oasis::BoxF& main, const oasis::BoxF& diff)
{
    std::vector<oasis::BoxF> result;
    std::vector<oasis::BoxF> tmp;

    double lim = std::numeric_limits<double>::max() / 2;

    tmp.reserve(4);
    tmp.push_back(oasis::Box(-lim, -lim, diff.left(), lim));
    tmp.push_back(oasis::Box(diff.right(), -lim, lim, lim));
    tmp.push_back(oasis::Box(diff.left(), diff.top(), diff.right(), lim));
    tmp.push_back(oasis::Box(diff.left(), -lim, diff.right(), diff.bottom()));

    for(std::vector<oasis::BoxF>::const_iterator it = tmp.begin(); it != tmp.end(); it++)
    {
        oasis::BoxF main_box = main;
        oasis::BoxF box= main_box.intersected(*it);
        if(!box.is_empty())
        {
            result.push_back(box);
        }
    }
    return result;
}


static Viewport empty_vp;
static Viewport& empty_vp_ref = empty_vp;

RenderManager::RenderManager (QObject* parent): ThreadManager(1), QObject(parent), m_frame(0), m_vp(empty_vp_ref),m_wakeup_times(0)
{
    m_init_update = false;
    m_resolution = 1.0;
    m_time = QTime::currentTime();
    m_cell_cache = new CellCacheManager();
}

RenderManager::~RenderManager()
{
    delete m_cell_cache;
}

void RenderManager::task_done(int index)
{

}

void RenderManager::save(Viewport& vp, double resoultion)
{
    m_trans = vp.trans();
    m_vp = vp;
    m_resolution = resoultion;
    oasis::BoxF new_area = vp.box();
    m_pre_center = oasis::PointF(new_area.left() + new_area.width() / 2, new_area.bottom() + new_area.height() / 2);
    m_saved_area = m_valid_area = new_area;
    m_pre_trans = m_trans;
}

void RenderManager::start(int threads, RenderFrame* frame, Viewport &vp, double resolution, bool force)
{
    m_frame = frame;
    m_vp = vp;
    m_vp.set_size(m_frame->width(), m_frame->height());
    m_resolution = resolution;
    m_trans = vp.trans();
    oasis::BoxF new_area = vp.box();
    double epsilon = m_trans.inverted().mag() * 1e-3;

    oasis::PointF shift;
    bool can_shift = false;
    int trans1 = compress_trans(m_pre_trans);
    int trans2 = compress_trans(m_trans);

//    oasis::PointF new_center(new_area.left() + new_area.width() / 2,
//                         new_area.bottom() + new_area.height() / 2);

//    if(!force && m_valid_area.overlap(new_area) && trans1 == trans2 &&
//       fabs(new_area.width() - m_saved_area.width()) < epsilon &&
//       fabs(new_area.height() - m_saved_area.height()) < epsilon)
//    {
//        oasis::Box full_box(0, 0, m_width, m_height);

//        std::vector<oasis::BoxF> box_vec = diff_box(new_area, m_valid_area);
//        m_redraw_area.clear();
//        m_redraw_area.reserve(box_vec.size());
//        for(std::vector<oasis::BoxF>::iterator it = box_vec.begin(); it != box_vec.end(); it++)
//        {
//            oasis::BoxF trans_box = it->transedF(m_trans);
//            oasis::Box box = oasis::Box(trans_box.left() - 1.0, trans_box.bottom() - 1.0, trans_box.right() + 1.0, trans_box.top() + 1.0);
//            box = box.intersected(full_box);
//            if(!box.is_empty())
//            {
//                m_redraw_area.push_back(box); //pixel space
//            }
//        }


//        shift = m_trans.transF(oasis::PointF(m_pre_center.x() - new_center.x(),
//                                             m_pre_center.y() - new_center.y()));
//        can_shift = true;
//    }
//    else
//    {
        m_redraw_area.clear();
        m_redraw_area.push_back(oasis::Box(0, 0, m_vp.width(), m_vp.height()));
        m_valid_area = m_saved_area = oasis::BoxF();
//    }

//    m_pre_center = new_center;

    std::vector<int> redraw_layer;
    prepare_render(true, can_shift, &shift,redraw_layer, threads);

}

void RenderManager::restart(RenderFrame* frame, const std::vector<int> &redraw_layers)
{
    m_redraw_area.clear();
    m_redraw_area.push_back(oasis::Box(0, 0, m_vp.width(), m_vp.height()));
    m_valid_area = m_saved_area = oasis::Box();
    m_frame = frame;

    oasis::PointF shift;
    prepare_render(false, false, &shift, redraw_layers, -1);
}

void RenderManager::prepare_render(bool clear, bool can_shift, const oasis::PointF *shift_disp, const std::vector<int> &redraw_layers, int threads)
{
    if(threads >= 0 && threads != thread_nums())
    {
        set_threads(threads );
    }

    m_init_update = true;

    int planes_per_layer = 3;
    size_t layer_nums = m_frame->layers_size();
    size_t planes_per_block = layer_nums * planes_per_layer;

    if(m_frame->layout_views_size() > 0)
    {
        if(clear)
        {
            std::vector<int> restart;
            m_frame->init_bitmaps(planes_per_block, m_vp.width(), m_vp.height(), m_resolution, can_shift, *shift_disp, false, restart);
        }
        else
        {
            std::vector<int> draw_planes;
            for(size_t i = 0; i < redraw_layers.size(); i++)
            {
                if(redraw_layers[i] >= 0 && redraw_layers[i] < (int)layer_nums)
                {
                    draw_planes.push_back(redraw_layers[i] * 3);
                    draw_planes.push_back(redraw_layers[i] * 3 + 1);
                    draw_planes.push_back(redraw_layers[i] * 3 + 2);
                }
            }
            m_frame->init_bitmaps(planes_per_block, m_vp.width(), m_vp.height(), m_resolution, can_shift, *shift_disp, true, draw_planes);
        }

        int block_row = 9;
        int block_column = 19;

        m_vp.set_size(m_vp.width(), m_vp.height());
        oasis::OasisTrans vp_trans = m_vp.trans();


        double left = std::numeric_limits<double>::max();
        double bottom = left;
        double right = std::numeric_limits<double>::min();
        double top = right;
        for(int i = 0; i < m_frame->layout_views_size(); i++)
        {
            render::LayoutView* tmp = m_frame->get_layout_view(i);
            oasis::OasisLayout* layout = tmp->get_layout();
            oasis::Box bound = layout->get_bbox();
            oasis::float64 dbu = layout->get_dbu();
            left = bound.left() * dbu< left ? bound.left() * dbu: left;
            bottom = bound.bottom() * dbu < bottom ? bound.bottom() * dbu : bottom;
            right = bound.right() * dbu > right ? bound.right() * dbu: right;
            top = bound.top() * dbu > top ? bound.top() * dbu : top;
        }

        oasis::BoxF overview_box(left, bottom, right, top);
        oasis::BoxF view_box = m_vp.box();

        std::vector<oasis::BoxF> split_micron_boxes;
        std::vector<oasis::Box> split_pixel_boxes;

        unsigned long levels =(unsigned long) (overview_box.width() * overview_box.height() / view_box.width() / view_box.height());

        bool local_fast_mode = false;
        if(view_box.width() * view_box.height() < 12.5)
        {
            block_column = 1;
            block_row = 1;
            local_fast_mode = true;
        }

        for(int i = 0; i < m_frame->layout_views_size(); i++)
        {
            render::LayoutView* tmp = m_frame->get_layout_view(i);
            oasis::OasisLayout* layout = tmp->get_layout();
            oasis::Box bound = layout->get_bbox();
            oasis::float64 dbu = layout->get_dbu();
            left = bound.left() * dbu< left ? bound.left() * dbu: left;
            bottom = bound.bottom() * dbu < bottom ? bound.bottom() * dbu : bottom;
            right = bound.right() * dbu > right ? bound.right() * dbu: right;
            top = bound.top() * dbu > top ? bound.top() * dbu : top;
            oasis::BoxF file_box(left, bottom, right, top);

            int block_width = 0;
            int block_height = 0;
            oasis::Point left_bottom(0, 0);
            oasis::Point right_top(block_width, block_height);
            int right_boundary = 0;
            int top_boundary = 0;
            if(include(file_box, view_box))
            {
                block_width = (m_vp.width() - 1)/ block_column;
                block_height = (m_vp.height() - 1)/ block_row;
                right_top.set_x(block_width);
                right_top.set_y(block_height);
                right_boundary = m_vp.width() - 1;
                top_boundary = m_vp.height() - 1;
            }
            else if(file_box.overlap(view_box))
            {
                oasis::BoxF inter_box_um = view_box.intersected(file_box);
                oasis::BoxF inter_box_p = inter_box_um.transedF(vp_trans);
                block_width = int(inter_box_p.width() / block_column);
                block_height = int(inter_box_p.height() / block_row);
                left_bottom.set_x((int) inter_box_p.left());
                left_bottom.set_y((int) inter_box_p.bottom());
                right_top.set_x(left_bottom.x() + block_width);
                right_top.set_y(left_bottom.y() + block_height);
                right_boundary = std::min( (unsigned int)inter_box_p.right() + 1, m_vp.width() - 1);
                top_boundary = std::min((unsigned int)inter_box_p.top() + 1, m_vp.height() - 1) ;
             }
            else
            {
                block_width = (m_vp.width() - 1)/ block_column;
                block_height = (m_vp.height() - 1)/ block_row;
                right_top.set_x(block_width);
                right_top.set_y(block_height);
                right_boundary = m_vp.width() - 1;
                top_boundary = m_vp.height() - 1;
            }

            for(int i = 0; i < block_row; i++)
            {
                for(int j = 0; j < block_column ;j++)
                {
                    oasis::Point tmp_left_bottom;
                    tmp_left_bottom.set_x(left_bottom.x() + j * block_width - 1);
                    tmp_left_bottom.set_y(left_bottom.y() + i * block_height - 1);

                    oasis::Point tmp_right_top;
                    int right_top_x = (j == (block_column - 1)) ? (right_boundary): (right_top.x() + j *  block_width);
                    int right_top_y = (i == (block_row - 1)) ? (top_boundary): (right_top.y() + i * block_height);
                    tmp_right_top.set_x(right_top_x + 1);
                    tmp_right_top.set_y(right_top_y + 1);

                    oasis::Box pixel_box(tmp_left_bottom.x(), tmp_left_bottom.y(), tmp_right_top.x(), tmp_right_top.y());
                    oasis::BoxF pixel_box_f(pixel_box);
                    oasis::OasisTrans pixel_to_micron_trans = vp_trans.inverted();
                    split_micron_boxes.push_back(pixel_box_f.transedF(pixel_to_micron_trans));
                    split_pixel_boxes.push_back(pixel_box);
                }
            }
        }

        std::vector<render::LayoutView*> lvs = m_frame->get_layout_views_list();
        std::set<int> lv_index_group;
        std::map<render::LayoutView*, std::vector<std::pair<oasis::LDType, int> > > lv_to_lds_map;

        if(clear)
        {
            for(size_t i = 0; i < m_frame->layers_size(); i++)
            {
                const render::LayerProperties* lp = m_frame->get_properties(i);
                 m_frame->set_layer_cached(i, false);
                if(lp->visible())
                {
                    render::LayerMetaData l = lp->metadata();
                    oasis::LDType ld(l.get_layer_num(), l.get_data_type());
                    int lv_index = lp->view_index();
                    lv_to_lds_map[lvs[lv_index]].push_back(std::make_pair(ld, i));
                    lv_index_group.insert(lv_index);
                     m_frame->set_layer_cached(i, true);
                }
            }
        }
        else
        {
            for(size_t i = 0; i < redraw_layers.size(); i++)
            {
                const render::LayerProperties* lp = m_frame->get_properties(i);
                m_frame->set_layer_cached(i, false);
                if(lp->visible())
                {
                    render::LayerMetaData l = lp->metadata();
                    oasis::LDType ld(l.get_layer_num(), l.get_data_type());
                    int lv_index = lp->view_index();
                    lv_to_lds_map[lvs[lv_index]].push_back(std::make_pair(ld, i));
                    lv_index_group.insert(lv_index);
                    m_frame->set_layer_cached(i, true);
                }
            }
        }

        if(lv_index_group.empty())
        {
            return;
        }

        if(local_fast_mode)
        {
            for(size_t i = 0; i < split_micron_boxes.size(); i++)
            {
                oasis::BoxF micron_box_f = split_micron_boxes[i];
                oasis::Box pixel_box = split_pixel_boxes[i];
                for(auto it = lv_to_lds_map.begin(); it != lv_to_lds_map.end(); it++)
                {
                    std::vector<std::pair<oasis::LDType, int> >& layers= it->second;
                    for(size_t j = 0; j < layers.size(); j++)
                    {
                        std::map<render::LayoutView*, std::vector<std::pair<oasis::LDType, int> > > tmp;
                        tmp[it->first].push_back(std::make_pair(layers[j].first, 0));
                        std::vector<render::LayoutView*>  lvs_tmp;
                        lvs_tmp.push_back(it->first);
                        RenderTask *task = new RenderTask(i, levels, 3, micron_box_f,pixel_box, m_trans, tmp, lvs_tmp, true, layers[j].second);
                        push(static_cast<Task*>(task));
                    }
                }
            }
        }
        else
        {
            for(size_t i = 0; i < split_micron_boxes.size(); i++)
            {
                oasis::BoxF micron_box_f = split_micron_boxes[i];
                oasis::Box pixel_box = split_pixel_boxes[i];
                RenderTask* task = new RenderTask(i, levels, planes_per_block, micron_box_f, pixel_box, m_trans, lv_to_lds_map, lvs, false, -1);
                push(static_cast<Task*>(task));
            }
        }
    }
    else
    {
        m_frame->init_bitmaps(1, m_vp.width(), m_vp.height(), m_resolution, false, *shift_disp, false, redraw_layers);
    }

    QString debug_info;
    debug_info = QString("ready to start thread manager");
    logger_file(debug_info);
//    m_main_timer.start();
    ThreadManager::start();

    m_initial_wait_lock.lock();
    if(m_init_update && clear)
    {
        debug_info = QString("wait the initail update");
        logger_file(debug_info)
        m_initial_wait_cond.wait(&m_initial_wait_lock);
    }
    m_init_update = false;
    m_initial_wait_lock.unlock();

}

RenderThread* RenderManager::create_thread()
{
    return new RenderLayerThread(this);
}

void RenderManager::setup_thread(RenderThread *thread)
{
    RenderLayerThread* render_thread =  dynamic_cast<RenderLayerThread*> (thread);
    if(render_thread)
    {
        render_thread->setup(m_frame, m_redraw_area, m_cell_cache);
    }
}

void RenderManager::finished()
{
    oasis::PointF left_bot = m_trans.inverted().transF(oasis::PointF(0, 0));
    oasis::PointF right_top = m_trans.inverted().transF(oasis::PointF(m_vp.width(), m_vp.height()));
    m_saved_area = oasis::BoxF(left_bot.x(), left_bot.y(), right_top.x(), right_top.y());
    m_valid_area = m_saved_area;
    end();
}

void RenderManager::closed()
{
    oasis::PointF left_bot = m_trans.inverted().transF(oasis::PointF(0, 0));
    oasis::PointF right_top = m_trans.inverted().transF(oasis::PointF(m_vp.width(), m_vp.height()));
    m_saved_area = oasis::BoxF(left_bot.x(), left_bot.y(), right_top.x(), right_top.y());
    m_valid_area.intersect(m_saved_area);
    end();
}

void RenderManager::end()
{
    QString debug_info;
    debug_info = QString("end function called");
    logger_file(debug_info)
//    m_main_timer.stop();
    wakeup();

    for(int i = 0; i < thread_nums(); i++)
    {
        RenderLayerThread* thread = dynamic_cast<RenderLayerThread*> (get_thread(i));
        if(thread)
        {
            //cleanup the buffer per thread.
            thread->finish();
        }
    }

    debug_info = QString("ready to call cell cache clean up");
    logger_file(debug_info);
    m_cell_cache->cleanup();
    debug_info = QString("finish call cell cache clean up");
    logger_file(debug_info);
    m_frame->finish_drawing();
}

static double calculate_mesc(QTime& t1, QTime& t2)
{
    double result = 0.0;

    if(t1.hour() - t2.hour() != 0)
    {
        double pre_time = t1.minute() * 60 + t1.second() + t1.msec() * 0.0001;
        double cur_time = (t2.minute() + 60) * 60  + t2.second() + t2.msec() * 0.001;
        result = cur_time - pre_time;
    }
    else
    {
        double pre_time = t1.minute() * 60 + t1.second() + t1.msec() * 0.001;
        double cur_time = t2.minute() * 60 + t2.second() + t2.msec() * 0.001;
        result = cur_time - pre_time;
    }
    return result;

}

void RenderManager::wakeup_inspected()
{
    QTime current_time = QTime::currentTime();
    if(calculate_mesc(m_time, current_time) >= checkpoint_interval * 0.001)
    {
        wakeup();
    }
}

void RenderManager::wakeup()
{
    bool update_image = false;
    m_initial_wait_lock.lock();
//    QString debug_info;
    if(m_init_update)
    {
//        debug_info = QString("wake up the first time");
//        logger_file(debug_info)
        m_initial_wait_cond.wakeAll();
        m_init_update = false;
    }
    else
    {
        update_image = true;
    }

    m_initial_wait_lock.unlock();

    if(update_image)
    {
//        debug_info = QString("update image");
//        logger_file(debug_info)
        m_frame->update_image();
    }
}

RenderLayerThread::RenderLayerThread(RenderManager* manager):m_manager(manager),m_hit_counts(0), m_x_offset(0),m_y_offset(0), m_fast_mode(false)
{
    m_time = QTime::currentTime();
}

void RenderLayerThread::cleanup()
{
    for(size_t i = 0; i < m_buffer.size(); i++)
    {
        delete m_buffer[i];
        m_buffer[i] = 0;
    }
    std::vector<render::Bitmap*>().swap(m_buffer);
}

RenderLayerThread::~RenderLayerThread()
{
    cleanup();
}

void RenderLayerThread::setup(RenderFrame* frame, const std::vector<oasis::Box>& redraw_area, CellCacheManager* cache)
{
    m_frame = frame;
    m_redraw_area = redraw_area;
    m_cache = cache;
}

void RenderLayerThread::finish()
{
    cleanup();
}

void RenderLayerThread::do_task(Task *task)
{
    RenderTask* render_task = dynamic_cast<RenderTask*>(task);
    if(!render_task)
    {
        return;
    }

//    QString debug_info;
//    debug_info = QString("begin to do task, thread id:%0").arg(QThread::currentThreadId());
//    logger_file(debug_info);
    set_idle(false);
//    debug_info = QString("ready to clean up the buffer, thread id:%0").arg(QThread::currentThreadId());
//    logger_file(debug_info);
    cleanup();
//    debug_info = QString("finish to cleanup the buffer, thread id:%0").arg(QThread::currentThreadId());
//    logger_file(debug_info);

    int m_block_index = render_task->m_block_index;
    unsigned long levels = render_task->m_levels;
    int planes_per_block =  render_task->m_planes;
    oasis::BoxF& m_micron_box = render_task->m_micron_box;
    oasis::Box& m_pixel_box = render_task->m_pixel_box;
    oasis::OasisTrans& m_trans = render_task->m_trans;

    std::map<LayoutView*, std::vector<std::pair<oasis::LDType, int> > >& m_lv_map = render_task->m_lv_map;
    std::vector<LayoutView*>& m_lvs = render_task->m_lvs;

    m_fast_mode = render_task->m_fast_mode;
    int m_layer_index = render_task->m_layer_index;

    for(int i = 0; i < planes_per_block; i++)
    {
        m_buffer.push_back(new Bitmap(m_pixel_box.width(), m_pixel_box.height(), 1.0));
    }

    m_redraw_layers.clear();
    std::vector<int>().swap(m_redraw_layers);
    m_x_offset = m_pixel_box.left();
    m_y_offset = m_pixel_box.bottom();
//    QTime t;
//    t.start();

    for(size_t i = 0; i < m_lvs.size(); i++)
    {
        render::LayoutView* lv = m_lvs[i];
        oasis::OasisLayout* layout = lv->get_layout();

        oasis::float64 dbu = lv->get_dbu();
        oasis::OasisTrans dbu_trans(false, 0.0, dbu, oasis::PointF(0.0, 0.0));
        oasis::BoxF dbu_box_f = m_micron_box.transed(dbu_trans.inverted());
        oasis::Box box(dbu_box_f);
        oasis::OasisTrans global_to_local_trans(false, 0.0, 1.0, oasis::PointF(-m_pixel_box.left(), - m_pixel_box.bottom()));
        oasis::OasisTrans trans = global_to_local_trans * m_trans * dbu_trans;

        auto it = m_lv_map.find(lv);
        if(it == m_lv_map.end())
        {
            continue;
        }
        std::vector<std::pair<oasis::LDType, int> > tmp = it->second;
//        std::map<oasis::LDType, int> ld_map;

        std::unordered_map<oasis::LDType, int, hash_LDType, cmp_LDType> ld_map;
        if(m_fast_mode)
        {
            ld_map[tmp[0].first] = tmp[0].second;
            m_redraw_layers.push_back(m_layer_index);
        }
        else
        {
            for(size_t j = 0; j < tmp.size(); j++)
            {
                ld_map[tmp[j].first] = tmp[j].second;
                m_redraw_layers.push_back(tmp[j].second);
            }
        }
//        QString info = QString("ready to read file, block_index %0, thread id: %1").arg(m_block_index).arg(QThread::currentThreadId());
//        logger_file(info);
        layout->draw_bitmap(-1, levels, m_block_index, box, trans, dbu_trans, ld_map, m_buffer, m_cache, this);
    }

//    qDebug() << "block index:" << m_block_index << m_micron_box.left() << m_micron_box.bottom() << m_micron_box.right() << m_micron_box.top();
//    QString time_info = QString("finish block %0,  use time: %1 ms").arg(m_block_index).arg(t.elapsed());
//    ui::logger_widget(time_info);
//   logger_file(info);
    copy_bitmaps();
    m_manager->wakeup();
//    QString info = QString("copy block %0 bitmap").arg(m_block_index);
//    logger_file(info);
    set_idle(true);

}

void RenderLayerThread::copy_bitmaps()
{
    if(m_fast_mode)
    {
        if(m_redraw_layers.size() <= 1)
        {
            m_frame->copy_bitmap(m_redraw_layers[0], m_buffer[0], m_buffer[1], m_x_offset, m_y_offset);
        }
        else
        {
            qDebug() << "Unexpected error.";
        }
    }
    else
    {
        m_frame->copy_bitmaps(m_redraw_layers, m_buffer, m_x_offset, m_y_offset);
    }
}

void RenderLayerThread::snapshot()
{
    sentinel();
    if(m_manager->thread_nums() > 0)
    {
        if(m_hit_counts == 100)
        {
            m_hit_counts = 0;
            QTime time = QTime::currentTime();
            if(calculate_mesc(time, m_time) > checkpoint_interval * 0.001)
            {
                copy_bitmaps();
                m_manager->wakeup_inspected();
                m_time = time;
            }
        }
        else
        {
            m_hit_counts++;
        }
     }
}
}
#endif // RENDER_WORKER_CC
