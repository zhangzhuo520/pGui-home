#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include "oasis_layout.h"
#include "render_multithread_manager.h"

#include <QTime>
#include <QTimer>
#include <QRunnable>

namespace render{

class Bitmap;
class LayoutView;
class Viewport;
class RenderFrame;
class CellCacheManager;

const int checkpoint_interval = 500;

class RenderManager:public ThreadManager, public QObject
{
public:
    RenderManager(QObject* parent = nullptr);
    virtual ~RenderManager();
    void start(int threads, RenderFrame* frame, render::Viewport &vp, double resolution, bool force);
    void restart(RenderFrame* frame, const std::vector<int>& restart_threads);
    void wakeup_inspected();
    void wakeup();

    void save(render::Viewport& vp, double resolution);
    void task_done(int index);

protected:
    RenderThread* create_thread();
    void setup_thread(RenderThread *thread);

    void finished();
    void closed();

private:
    void prepare_render(bool clear, bool can_shift, const oasis::PointF* shift_disp, const std::vector<int>& redraw_layer, int thread_nums);
    void end();
    bool m_init_update;
    std::vector<oasis::Box> m_redraw_area;

    oasis::int64 m_infoindex;
    oasis::Box m_q_box;
    oasis::OasisTrans m_trans;
    oasis::float64 m_resolution;

    RenderFrame* m_frame;
    render::Viewport& m_vp;
    std::vector<oasis::Box> m_render_areas;
    oasis::BoxF m_saved_area, m_valid_area;

    oasis::PointF m_pre_center;
    oasis::OasisTrans m_pre_trans;

    QMutex m_initial_wait_lock;
    QWaitCondition m_initial_wait_cond;

    QTime m_time;
    QTimer m_main_timer;

    int m_wakeup_times;

    //maybe smart prt will be more useful.
    CellCacheManager* m_cell_cache;
//    QMutex m_locker;
//    int m_use_count;
//    static RenderManager* m_instance;
};

class RenderLayerThread:public RenderThread
{
public:
    RenderLayerThread(RenderManager* manager);

    virtual ~RenderLayerThread();

    void setup(RenderFrame* frame, const std::vector<oasis::Box>& redraw_area, CellCacheManager* cache);

    void finish();
    void snapshot();

protected:
    virtual void do_task(Task* task);

private:

    void cleanup();
    void copy_bitmaps();

    RenderManager* m_manager;
    std::vector<oasis::Box> m_redraw_area;
    RenderFrame* m_frame;
    QTime m_time;
    int m_hit_counts;
    std::vector<render::Bitmap*> m_buffer;
    std::vector<int> m_redraw_layers;
    int m_x_offset;
    int m_y_offset;
    bool m_fast_mode;
    CellCacheManager* m_cache;

};

class RenderTask: public Task
{
public:
    RenderTask(int block_index,
               unsigned long levels,
               int planes,
               oasis::BoxF micron_box,
               oasis::Box pixel_box,
               oasis::OasisTrans trans,
               std::map<LayoutView*, std::vector<std::pair<oasis::LDType, int> > >& lv_map,
               std::vector<LayoutView*>& lvs,
               bool fast_mode,
               int layer_index)
               :m_block_index(block_index),
               m_levels(levels),
               m_planes(planes),
               m_micron_box(micron_box),
               m_pixel_box(pixel_box),
               m_trans(trans),
               m_lv_map(lv_map),
               m_lvs(lvs),
               m_fast_mode(fast_mode),
               m_layer_index(layer_index){ }

    int m_block_index;
    unsigned long m_levels;
    int m_planes;
    oasis::BoxF m_micron_box;
    oasis::Box m_pixel_box;
    oasis::OasisTrans m_trans;

    std::map<LayoutView*, std::vector<std::pair<oasis::LDType, int> > > m_lv_map;
    std::vector<LayoutView*> m_lvs;
    bool m_fast_mode;
    int m_layer_index;
};

}
#endif // RENDER_WORKER_H
