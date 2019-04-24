#include "render_bitmap_manager.h"
#include "render_bitmaps_to_image.h"
#include "render_pattern.h"
#include "render_line_style.h"
#include "render_view_op.h"

#include <QImage>
namespace render
{

static void move_bitmap(Bitmap* from, Bitmap* to, const oasis::Point& shift_disp)
{
    to->clear();
    int x_offset = shift_disp.x();
    int y_offset = shift_disp.y();

    if(y_offset <= -int(from->height()) || y_offset >= int(from->height()) ||
       x_offset <= -int(from->width()) || x_offset >= int(from->width()))
    {
        return;
    }

    // from bitmap needs to transfer the data from line to boundary
    int line_bound = int(to->height()) - std::max(0,y_offset);
    for(int cur_line = std::max(y_offset, 0); cur_line < line_bound; cur_line++)
    {
        if(from->is_empty_scanline(cur_line))
        {
            continue;
        }

        const uint32_t* scanline_from = from->scanline(cur_line);
        uint32_t* scanline_to = to->scanline(cur_line + y_offset);

        if(x_offset < 0)
        {
            unsigned int start_segment = ((unsigned int) -x_offset) / 32;
            unsigned int segments = (to->width() + 31) / 32 - start_segment;
            scanline_from += start_segment;

            unsigned int left_offset = ((unsigned int ) -x_offset) % 32;
            if(left_offset == 0)
            {
                for(unsigned int i = 0; i < segments; ++i)
                {
                    *scanline_to++ = *scanline_from++;
                }
            }
            else
            {
                unsigned int right_offset = 32 - left_offset;
                for(unsigned int i = 1; i < segments; i++)
                {
                    *scanline_to ++ = (scanline_from[0] >> left_offset) | (scanline_from[1] << right_offset);
                    scanline_from++;
                }
                if(segments)
                {
                    *scanline_to++ = scanline_from[0] >> left_offset;
                }
            }
        }
        else
        {
            unsigned int start_segment = ((unsigned int) x_offset ) / 32;
            unsigned int segments = (to->width() + 31) / 32 - start_segment;
            scanline_to += start_segment;

            unsigned int left_offset = ((unsigned int) x_offset) % 32;

            if(left_offset == 0)
            {
                for(unsigned int i = 0 ; i < segments; i++)
                {
                    *scanline_to++ = *scanline_from++;
                }
            }
            else
            {
                unsigned int right_offset = 32 - left_offset;
                if(segments)
                {
                    *scanline_to++ = scanline_from[0] << left_offset;
                }
                for(unsigned int i = 1; i < segments; i++)
                {
                    *scanline_to++ = (scanline_from[0] >> right_offset) | (scanline_from[1] << left_offset);
                    scanline_from++;
                }
            }
        }
    }

}

BitmapManager::~BitmapManager()
{
    clear_bitmaps();
}

bool BitmapManager::is_empty_bitmap(int index)
{
    bool result = true;
    lock();
    if(index >= 0 && (size_t)index < m_bitmap_buffer.size())
    {
        result = m_bitmap_buffer[index]->empty();
    }
    unlock();
    return result;
}

void BitmapManager::init_bitmaps(unsigned int planes, unsigned int width, unsigned int height, double resolution,
                            bool shift, const oasis::Point &shift_disp,
                            bool restart, const std::vector<int> & restart_bitmaps)
{
    m_width = width;
    m_height = height;
    m_resolution = resolution;

    lock();

    if(shift)
    {
        for(size_t i = 0; i < m_bitmap_buffer.size(); i++)
        {
            Bitmap* ori =  m_bitmap_buffer[i];
            Bitmap* cur =  new Bitmap(m_width, m_height, m_resolution);
            m_bitmap_buffer[i] = cur;
            move_bitmap(ori, cur, shift_disp);
            delete ori;
            ori = 0;
        }
    }
    else if(restart)
    {
        for(size_t i = 0; i< restart_bitmaps.size(); i++)
        {
            if(restart_bitmaps[i] >= 0 && restart_bitmaps[i] <= (int)m_bitmap_buffer.size())
            {
                m_bitmap_buffer[restart_bitmaps[i]]->clear();
            }
        }
    }
    else
    {
        m_width = width;
        m_height = height;

        clear_bitmaps();
        for(unsigned int i = 0; i < planes; i++)
        {
            m_bitmap_buffer.push_back(new Bitmap(width, height, resolution));
        }
    }
    unlock();

}

void BitmapManager::set_bitmap(int index, Bitmap *bitmap)
{
    lock();
    if(bitmap != 0 && index >= 0 && index < (int)m_bitmap_buffer.size())
    {
        *(m_bitmap_buffer[index]) = *bitmap;
    }
    unlock();
}


void BitmapManager::clear_bitmaps()
{
    while(!m_bitmap_buffer.empty())
    {
        delete m_bitmap_buffer.back();
        m_bitmap_buffer.pop_back();
    }
    std::vector<render::Bitmap*>().swap(m_bitmap_buffer);
}

Bitmap* BitmapManager::create_bitmap()
{
    return new Bitmap(m_width, m_height, m_resolution);
}

void BitmapManager::copy_bitmap(Bitmap *to, int index)
{
    lock();
    if(to != 0 && index >= 0 && index <(int) m_bitmap_buffer.size())
    {
        *to = *(m_bitmap_buffer[index]);
    }
    unlock();
}

void BitmapManager::copy_bitmaps_with_shift(const std::vector<int>& need_layers, const std::vector<render::Bitmap*>& bitmaps, int x, int y)
{
    lock();
    for(size_t i = 0; i < need_layers.size(); i++)
    {
        int layer_index = need_layers[i];
        render::Bitmap* contour_from = bitmaps[layer_index * 3];
        render::Bitmap* contour_to = m_bitmap_buffer[layer_index * 3];
        render::Bitmap* fill_from = bitmaps[layer_index * 3 + 1];
        render::Bitmap* fill_to = m_bitmap_buffer[layer_index * 3 + 1];
        contour_to->merge(contour_from, x, y);
        fill_to->merge(fill_from, x, y);
    }
    unlock();
}

void BitmapManager::copy_bitmap_with_shift(int index, render::Bitmap* bitmap_from, int x, int y)
{
    lock();
    render::Bitmap* bitmap_to = m_bitmap_buffer[index];
    bitmap_to->merge(bitmap_from, x, y);
    unlock();
}

void BitmapManager::merge_to_image(const std::vector<ViewOp>& view_ops, const Pattern& pattern, const LineStyle& line_style, QImage* image, unsigned int width, unsigned int height)
{
    bitmaps_to_image(view_ops, m_bitmap_buffer, pattern, line_style, image, width, height, &m_mutex);
}

}
