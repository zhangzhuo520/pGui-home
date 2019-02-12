#include "render_bitmaps_to_image.h"

#include <stdint.h>
#include <QMutex>
#include <QImage>
#include "render_bitmap.h"
#include "render_pattern.h"
#include "render_line_style.h"

namespace  render {

static void 
render_scanline_std(
    const render::Bitmap *bitmap,
    unsigned int y,
    unsigned int width,
    unsigned int height,
    uint32_t * buffer)
{
    const uint32_t* sl = bitmap-> scanline(y);
    unsigned int x = width;
    for (; x >= render::wordlen; x -= render::wordlen)
    {
        *buffer ++ = *sl ++;
    }
    if (x > 0)
    {
        *buffer = *sl;
    }
}

static void 
render_scanline_default(
    const uint32_t* pattern,
    unsigned int stride,
    const render::Bitmap * bitmap,
    unsigned int y,
    unsigned int width,
    unsigned int  height,
    uint32_t * buffer)
{
    const uint32_t * sl = bitmap->scanline(y);
    const uint32_t * iter = pattern;
    unsigned int x = width;
    for (; x >= render::wordlen ; x-= render::wordlen)
    {
        *buffer++ = *sl++ & *iter++;
        if(iter == pattern + stride)
        {
            iter = pattern;
        }
    }

    if(x > 0)
    {
        *buffer = *sl & *iter;
    }

}

static void
render_scanline_edge(
    const uint32_t* pattern,
    unsigned int stride,
    const render::Bitmap* bitmap,
    unsigned int y,
    unsigned int width,
    unsigned int height,
    uint32_t* buffer)
{
    const uint32_t* top_line = ( y < (height - 1) ? bitmap->scanline(y + 1): bitmap->empty_scanline());
    const uint32_t* bot_line = ( y > 0 ? bitmap->scanline(y - 1): bitmap->empty_scanline());
    const uint32_t* cur_line = bitmap->scanline(y);

    const uint32_t* pattern_iter = pattern;
    
    unsigned int pos = ( y % (32 * stride));
    
    bool v_flag = (pattern[pos /32] & (1 <<( pos % 32 ))) != 0;

    uint32_t data_pre = 0;

    int xbits = width;

    while(xbits >=0)
    {
        uint32_t data = 0;
        uint32_t data_top = 0, data_bot = 0;
        uint32_t data_next = 0;
        
        if( xbits > int(render::wordlen))
        {
            data = *cur_line++;
            data_top = *top_line++;
            data_bot = *bot_line++;
            data_next = *cur_line;
        }
        else
        {
            data = *cur_line;
            data_top = *top_line;
            data_bot = *bot_line;
            
            if(xbits < int(render::wordlen))
            {
                data &= (1 << xbits) - 1;
            }
        }
        
        uint32_t data_hor_right =  (data & ((data >> 1) | ( (data_next & 1) << 31)));
        uint32_t data_hor_left = (data & ( (data << 1) | ((data_pre >> 31) & 1 )));
        uint32_t data_hor_and = data_hor_right & data_hor_left;
        uint32_t data_hor_or = data_hor_right | data_hor_left;
        
        uint32_t data_ver1 = data_top & data;
        uint32_t data_ver2 = data_bot & data;
        uint32_t data_ver_and = data_ver1 & data_ver2;
        uint32_t data_ver_or = data_ver1 | data_ver2;

        
        uint32_t solo     = data - (data & (data_hor_and | data_hor_or | data_ver_and | data_ver_or));
        uint32_t hor_mask = (data - (data & data_ver_and)) & data_hor_or;
        uint32_t ver_mask = (data - (data & data_hor_and)) & data_ver_or;
        uint32_t hor_and_ver_mask = data & data_hor_and & data_hor_or & data_ver_and & data_ver_or;
    
        uint32_t line_pattern = *pattern_iter++;
        uint32_t result = (solo | hor_mask) & line_pattern;
        if(v_flag)
        {
            result |=  ver_mask | (hor_and_ver_mask & line_pattern);
        }
        
        *buffer++ = result;
        if(pattern_iter == pattern + stride)
        {
            pattern_iter = pattern;
        }

        xbits -= render::wordlen;
        data_pre = data;
    }
}

static void render_scanline_line_width(
    const uint32_t* pattern,
    unsigned int stride,
    const render::Bitmap* bitmap,
    unsigned int y,
    unsigned int width,
    unsigned int height,
    uint32_t* buffer_ptr,
    unsigned int line_width)
{
    if(line_width < 1)
    {
        return;
    }

    if(line_width > 15)
    {
        line_width = 15;
    }

    const uint32_t* pattern_ptr = pattern;
    unsigned int prev_line_width =  (line_width - 1) / 2;
    unsigned int lat_line_width = line_width - 1 - prev_line_width;

    const uint32_t* buffer[16];

    for(unsigned int i = 0; i < line_width; i++)
    {
        if( y + i < prev_line_width)
        {
            buffer[i] =  bitmap->scanline(0);
        }
        else if( y + i - prev_line_width >= height)
        {
            buffer[i] = bitmap->scanline(height - 1);
        }
        else
        {
            buffer[i] = bitmap->scanline(y + i - prev_line_width);
        }
    }

    uint32_t prev = 0;
    uint32_t current = 0;
    uint32_t next = 0;

    //vertical_direction
    for(unsigned int i = 0; i < line_width; i++)
    {
        next |= *buffer[i];
        buffer[i] += 1;
    }

    while(true)
    {
        current = next;
        next = 0;

        if( width >= render::wordlen)
        {
            for(unsigned int i = 0; i < line_width; i++)
            {
                next |=  *buffer[i];
                buffer[i] ++;
            }
        }

        uint32_t original  = current;

        for(unsigned int i = 1; i <= prev_line_width; i++)
        {
            current |= (original >> i);
            current |= (next << (32 - i));
        }

        for(unsigned int i = 1; i <= lat_line_width; i++)
        {
            current |= (original << i);
            current |= (prev >> (32 - i));
        }

        *buffer_ptr++ = current & *pattern_ptr ++;
        if(pattern_ptr == pattern + stride)
        {
            pattern_ptr = pattern;
        }

        prev = original;

        if( width >= render::wordlen)
        {
            width-= render::wordlen;
        }
        else{
            break;
        }

    }

}

static void 
bitmaps_to_image_rgb(
    const std::vector<render::ViewOp> view_ops_in,
    const std::vector<render::Bitmap *> &pbitmap_in,
    const render::Pattern &dp,
    const render::LineStyle &ls,
    QImage* pimage_in,
    unsigned int width,
    unsigned int height,
    bool transparent,
    QMutex* mutex)
{
    unsigned int size = view_ops_in.size();
    std::vector<const render::Bitmap*> bitmaps;
    std::vector<uint32_t> non_empty_scanline;
    std::vector<render::ViewOp> view_ops;
    std::vector<std::pair<render::color_t, render::color_t> > masks;
    
    bitmaps.reserve(size);
    view_ops.reserve(size);
    non_empty_scanline.reserve(size);
    masks.reserve(size);
    
    unsigned int lines = 32;
    unsigned int nwords = (width + 31) /32 ;
    uint32_t * buffer = new uint32_t[size * nwords];

    for(unsigned int y = 0; y < height; y++)
    {
        if(mutex)
        {
            mutex->lock();
        }

        if(y % lines == 0)
        {
            view_ops.erase(view_ops.begin(), view_ops.end());
            bitmaps.erase(bitmaps.begin(), bitmaps.end());
            non_empty_scanline.erase(non_empty_scanline.begin(), non_empty_scanline.end());
            
            for(unsigned int i = 0; i< size; i++)
            {

                const render::ViewOp &op = view_ops_in[i];
                int bitmap_index = -1;
                if(op.bitmap_index() != -1)
                {
                    bitmap_index = op.bitmap_index();
                }
                else
                {
                    bitmap_index = i;
                }
                const render::Bitmap *pb = pbitmap_in[bitmap_index];
                if(pb != 0  &&(pb->first_scanline () < y + lines && pb->last_scanline () > y) && ((op.ormask() | ~op.andmask()) != 0))
                {
                    uint32_t non_empty_sl = 0;
                    uint32_t m = 1;
                    for(unsigned int yy = 0; yy < lines && yy + y <height ; ++yy, m<<= 1)
                    {
                        if(!pb->is_empty_scanline(yy + y))
                        {
                            non_empty_sl |= m;
                        }
                    }

                    if(non_empty_sl)
                    {
                        view_ops.push_back(op);
                        bitmaps.push_back(pb);
                        non_empty_scanline.push_back(non_empty_sl);    
                    }
                }
            }
        }

        masks.erase(masks.begin(), masks.end());

        const uint32_t needed_bits = 0x00ffffff;// alpha channel not needed
        const uint32_t fill_bits   = 0xff000000;
        uint32_t *buffer_p = buffer;

        uint32_t row_in_slice = (1 << (y %lines));
        for(unsigned int i = 0 ; i < view_ops.size() ; i++)
        {
            const render::ViewOp &op = view_ops[i];
            if( (non_empty_scanline[i] & row_in_slice) != 0)
            {
                const render::LineStyleInfo &li = ls.style(op.line_style_index());
                const render::PatternInfo &pi = dp.pattern(op.pattern_index());
                const uint32_t * pattern = pi.pattern()[y % pi.height()];
                if(pattern != 0)
                {
                    unsigned int stride = pi.stride();
                    masks.push_back(std::make_pair(op.ormask() & needed_bits, 
                                    ~op.ormask() & op.andmask() & needed_bits)) ;
                    
                    if(op.width() == 1)
                    {
                        if(li.width() > 0)
                        {
                            render_scanline_edge(li.pattern(), li.stride(), bitmaps[i], y, width, height, buffer_p);
                        }
                        else
                        {
                            render_scanline_default(pattern, stride, bitmaps[i], y, width, height, buffer_p);
                        }
                    }
                    else  if(op.width() > 1)
                    {
                        if(op.shape() == render::ViewOp::Rect)
                        {
                            render_scanline_line_width(pattern,stride, bitmaps[i], y, width, height, buffer_p, op.width());
                        }
                    }
                    buffer_p += nwords;
                }
            }
        }


        if(mutex)
        {
            mutex->unlock();
        }

        if(masks.size() > 0)
        {
            render::color_t *pt = (render::color_t *) pimage_in->scanLine(height - 1 - y);
            uint32_t* buffer_end = buffer_p;
        
            unsigned int i = 0;
        
            for(unsigned int x = 0; x < width; x+= 32 ,++i)
            {
                render::color_t y[32];
                if(transparent)
                {
                    for( int i = 0; i < 32 ; i++)
                    {
                        y[i] = 0;
                    }
                }
                else
                {
                    for(int i = 0; i < 32 ;i++)
                    {
                        y[i] = fill_bits;
                    }
                }

                render::color_t z[32] = {
                    render::wordones, render::wordones, render::wordones, render::wordones,
                    render::wordones, render::wordones, render::wordones, render::wordones,
                    render::wordones, render::wordones, render::wordones, render::wordones,
                    render::wordones, render::wordones, render::wordones, render::wordones,
                    render::wordones, render::wordones, render::wordones, render::wordones,
                    render::wordones, render::wordones, render::wordones, render::wordones,
                    render::wordones, render::wordones, render::wordones, render::wordones,
                    render::wordones, render::wordones, render::wordones, render::wordones
                };

                buffer_p = buffer_end - nwords + i;
                for(int j = masks.size() - 1; j >= 0; --j)
                {
                    uint32_t pix  = * buffer_p;
                    if(transparent)
                    {
                        uint32_t m = 1;
                        for(unsigned int k = 0; k < 32 && x + k < width; ++k , m <<= 1)
                        {
                            if ( (pix & m) != 0)
                            y[k] |=  (masks[j].first & z[k]) |  fill_bits;
                            z[k] &=  masks[j].second;
                        }
                    }
                    else
                    {
                        uint32_t m = 1;
                        for(unsigned int k = 0; k < 32 && x + k < width; ++k, m <<= 1)
                        {
                            if( (pix & m) != 0)
                            {
                                y[k] |=  masks[j].first & z[k];
                                z[k] &= masks[j].second;
                            }
                        }
                    }
                    buffer_p -= nwords;
                }        

                for(unsigned int k = 0; k < 32 && x + k < width; ++k)
                {
                    *pt  = (*pt & z[k]) | y[k];
                    pt ++;
                }
            }
        }
    }
}

static void
bitmaps_to_image_mono(
    const std::vector<render::Bitmap *> &pbitmap_in,
    QImage *pimage_in,
    unsigned int width,
    unsigned int height,
    QMutex *mutex)
{
    unsigned int size = pbitmap_in.size();
    std::vector<const render::Bitmap *> bitmaps;
    std::vector<uint32_t> non_empty_scanline;
    bitmaps.reserve(size);

    unsigned int lines = 32;
    unsigned int nwords = (width + 31) /32;
    uint32_t * buffer = new uint32_t[nwords * size];
    for(unsigned int y = 0; y < height; y++)
    {
        if(mutex)
        {
            mutex->lock();
        }

        if (y % lines == 0)
        {
            for(unsigned int i = 0 ;i < size; i++)
            {
                render::Bitmap *bp = pbitmap_in[i];
                if(bp != 0 && ( (bp-> first_scanline() < y + lines) && (bp->last_scanline() > y)))
                {
                    uint32_t m = 1;
                    uint32_t non_empty_sl = 0;
                    for (unsigned int yy = 0 ;yy < lines && yy + y < height; yy++, m <<= 1)
                    {
                        if(!bp->is_empty_scanline(yy))
                        {
                            non_empty_sl |= m;
                        }
                    }
                    non_empty_scanline.push_back(non_empty_sl);
                }
            }
        }

        uint32_t * buffer_p = buffer;
        Pattern p;
        PatternInfo pi = p.pattern(3);
        const uint32_t * pattern = pi.pattern()[y % pi.height()];
        if(pattern == 0) 
        {
            continue;
        }
        uint32_t stride = pi.stride();
        for(unsigned int i = 0 ; i < size ; i++)
        {
            if(i == 1 )
            {
                render_scanline_std(pbitmap_in[i], y, width,height, buffer_p);
                buffer_p += nwords;
            }
            else{
                render_scanline_default(pattern, stride, pbitmap_in[i], y, width, height, buffer_p);
                buffer_p += nwords;
            }
        }

        if(mutex)
        {
            mutex->unlock();
        }
        
        render::color_t *sl = (render::color_t *) pimage_in->scanLine(height - 1 - y);
        uint32_t *buffer_end = buffer_p;

        unsigned int i = 0;
        for(unsigned int x = 0; x < width ; x +=32, ++i)
        {
            uint32_t pixel = 0;
            //uint32_t z = lay::wordones;

            buffer_p = buffer_end - nwords + i;

            for(int j = size - 1; j >= 0; j--)
            {
                uint32_t data = *buffer_p;
                if(data != 0)
                {
                    uint32_t shift_op = 1;
                    for(int k = 0 ; k < 32 && k + x < width ; k++, shift_op <<= 1)
                    {
                        if( (data & shift_op) != 0)
                        {
                            pixel |= shift_op;
                        }
                    }
                }
                buffer_p -= nwords;
            }

            *sl |= pixel;
            sl++;
        }

    }
    delete[] buffer;
}

void bitmap_to_bitmap(const render::Bitmap &bitmap, unsigned char * data, unsigned int width, unsigned int height)
{
    //nothing yet
}


void bitmaps_to_image(
    const std::vector<render::ViewOp> &view_ops_in,
    const std::vector<render::Bitmap *> &pbitmaps,
    const render::Pattern &dp,
    const render::LineStyle &ls,
    QImage * pimage,
    unsigned int width,
    unsigned int height,
    QMutex * mutex)
{     
    if(pimage->depth() <= 1)
    {
        bitmaps_to_image_mono(pbitmaps, pimage, width, height, mutex);
    }
    else
    {
        bool transparent = (pimage->format() == QImage::Format_ARGB32);
        bitmaps_to_image_rgb(view_ops_in, pbitmaps, dp, ls,  pimage, width, height, transparent, mutex);
    }
}

}
