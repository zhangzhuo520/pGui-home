#ifndef RENDER_BITMAP_MANAGER_H
#define RENDER_BITMAP_MANAGER_H
#include "render_bitmap.h"
#include "oasis_types.h"
#include <QMutex>

class QImage;

namespace render
{

class Pattern;
class LineStyle;
class ViewOp;

class BitmapManager
{
public:
    BitmapManager(): m_resolution(1.0), m_width(0), m_height(0) {  }

    ~BitmapManager();

    void init_bitmaps(unsigned int layers, unsigned int width, unsigned int height, double resolution, bool shift, const oasis::Point &shift_disp,
                      bool restart, const std::vector<int> & need_restart_planes);

    void set_bitmap(int index, render::Bitmap* bitmap);

    render::Bitmap* create_bitmap();

    void copy_bitmap(Bitmap* to, int index);

    void lock()
    {
        m_mutex.lock();
    }

    void unlock()
    {
        m_mutex.unlock();
    }

    QMutex& mutex()
    {
        return m_mutex;
    }

    double resoluton() const
    {
        return m_resolution;
    }

    unsigned int width() const
    {
        return m_width;
    }

    unsigned int height() const
    {
        return m_height;
    }

    bool is_empty_bitmap(int index) ;

    void copy_bitmaps_with_shift(const std::vector<int>& need_layers, const std::vector<render::Bitmap*>& bitmaps, int x, int y);

    void copy_bitmap_with_shift(int index, render::Bitmap* bitmap, int x, int y);

    void merge_to_image(const std::vector<ViewOp>& view_ops,
                        const Pattern& pattern,
                        const LineStyle& line_style,
                        QImage* image,
                        unsigned int width,
                        unsigned int height);

private:

    void clear_bitmaps();
    double m_resolution;
    unsigned int m_width;
    unsigned int m_height;
    QMutex m_mutex;
    std::vector<Bitmap*> m_bitmap_buffer;
};
}


#endif // RENDER_BITMAP_MANAGER_H
