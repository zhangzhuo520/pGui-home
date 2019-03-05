#include "render_bitmap.h"
#include <algorithm>

namespace render{
Bitmap::Bitmap() : m_empty_scanline(0)
{
    init(0, 0);
    m_resolution = 1.0;
}

Bitmap::Bitmap(unsigned int width, unsigned int height, double resolution)
    :m_empty_scanline(0)
{
    init(width, height);
    m_resolution = 1.0;
}

Bitmap::Bitmap(const Bitmap& bm):m_empty_scanline(0)
{
    init(bm.m_width, bm.m_height);
    operator=(bm);
}

Bitmap & Bitmap::operator= (const Bitmap& bm)
{
    if( &bm != this)
    {
        if(bm.m_width != m_width  || bm.m_height !=  m_height)
        {
            clean();
            init(bm.m_width, bm.m_height);
        }

        m_resolution = bm.m_resolution;

        unsigned int segments = (m_width + 31) /32;
        for (unsigned int i = 0 ; i < m_height ; i++)
        {
            if(bm.m_scanlines.size() != 0 && bm.m_scanlines[i] != 0)
            {
                uint32_t* s1 = m_scanlines[i];
                uint32_t* s2 = bm.m_scanlines[i];
                for(unsigned int j = 0 ; j < segments ; j++)
                {
                    *s1++ = *s2++;
                }
            }
            else if(bm.m_scanlines.size() != 0 && bm.m_scanlines[i] == 0)
            {
                m_free_scanlines.push_back(m_scanlines[i]);
                m_scanlines[i] = 0;
            }
        }
        m_first_line = bm.m_first_line;
        m_last_line = bm.m_last_line;
    }
    return *this;
}



Bitmap::~Bitmap()
{
    clean();
}


uint32_t* Bitmap::scanline(unsigned int n)
{
    if(m_scanlines.empty())
    {
        m_scanlines.resize(m_height, 0);
    }
    if(n >= m_scanlines.size())
    {
        return 0;
    }
    uint32_t seg_num = (m_width + 31) / 32;
    uint32_t* row_data = m_scanlines[n];
    
    if(row_data == 0)
    {
        if(!m_free_scanlines.empty())
        {
            m_scanlines[n] = m_free_scanlines.back();
            row_data = m_scanlines[n];
            m_free_scanlines.pop_back();
        }
        else
        {
            m_scanlines[n] = new uint32_t [seg_num];
            row_data = m_scanlines[n];
        }    
        for (uint32_t *p = row_data; seg_num > 0 ; --seg_num)
        {
            *p++ = 0;
        }

        if(m_first_line > n)
        {
            m_first_line = n;
        }

        if(m_last_line < n)
        {
            m_last_line = n;
        }
    }

    return row_data;
}

void Bitmap::clear()
{
    for(std::vector<uint32_t *>::iterator i = m_scanlines.begin(); i != m_scanlines.end(); i++)
    {
        if(*i)
        {
            m_free_scanlines.push_back(*i);
        }
    }

    for(std::vector<uint32_t *>::iterator i = m_scanlines.begin(); i != m_scanlines.end(); i++)
    {
        *i = 0;
    }

    m_first_line = 0;
    m_last_line = 0;
}

void Bitmap::clean()
{

    if(m_empty_scanline)
    {
        delete [] m_empty_scanline;
        m_empty_scanline = 0;
    }
    for(std::vector<uint32_t *>::iterator i = m_scanlines.begin(); i != m_scanlines.end(); i++)
    {
        delete[] *i;
        *i = 0;
    } 

    for(std::vector<uint32_t *>::iterator i = m_free_scanlines.begin(); i != m_free_scanlines.end(); i++)
    {
        delete[] *i;
        *i = 0;
    }

    m_first_line = m_last_line = 0;
    m_free_scanlines.clear();
    m_scanlines.clear();
}

void Bitmap::init(unsigned int width,unsigned int height)
{
    m_width = width;
    m_height = height;
    if(m_width > 0)
    {
        uint32_t seg_num = (m_width + 31) /32;
        m_empty_scanline = new uint32_t [seg_num];
        for (uint32_t *s = m_empty_scanline; seg_num > 0; seg_num--)
        {
            *s++ = 0;
        }
    }
    m_first_line = m_last_line = 0;
}

static const uint32_t mask[32] = {
0x00000000, 0x00000001, 0x00000003, 0x00000007,
0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff
};

static const uint32_t all_ones = 0xffffffff;


void Bitmap::fill(unsigned int y, unsigned int x1, unsigned int x2)
{
    unsigned int seg_start = x1 / 32;
    unsigned int seg_end = x2 / 32;
    unsigned int seg_start_pos =  x1 % 32;
    unsigned int seg_end_pos = x2 % 32;
    unsigned int seg_num = seg_end - seg_start;

    uint32_t * sl = scanline(y);
    sl += seg_start;
    
    if(seg_num == 0)
    {
        *sl |= mask[seg_end_pos] & (~mask[seg_start_pos]);
    }
    else if(seg_num > 0)
    {
        *sl++ |= ~mask[seg_start_pos];
        while(seg_num > 1)
        {
            *sl++ |= all_ones;
            seg_num --;
        }
        
        unsigned int m = mask[seg_end_pos];
        if(m)
        {
            *sl |= m;
        }
    }
}

void Bitmap::fill_brush(int y, int x, const uint32_t* p, uint32_t stride, unsigned int n)
{
    if(x < int(m_width))
    {
        for(unsigned int s = 0 ; s < stride; s++)
        {
            int x1 = x + s * 32;
            unsigned int p_data = *p++;
            if(p_data)
            {
                unsigned int bx = (unsigned int) x1 / 32;
                uint32_t* sl = scanline(y);
                sl += bx;
                *sl |= (p_data << (x1 - bx));
                
                if( (unsigned int) x1 > bx)
                {
                    bx += 32;
                    sl ++;
                    if(bx < m_width)
                    {
                        *sl |= (p_data >> (bx - (unsigned int) x1));
                    }
                }
            }
        }
    }
}


void Bitmap::render_vertices(std::vector<render::RenderEdge> &edges, int mode)
{
    double x_max = width();
    double y_max = height();
    
    for(std::vector<render::RenderEdge>::iterator e = edges.begin(); e != edges.end(); e++)
    {
        double x, y;
        if( mode == 0 || (*e).delta() > 0)
        {
            x = (*e).x1();
            y = (*e).y1();
            if(x >= 0.0 && x < x_max && y >= 0.0 && y < y_max)
            {
                unsigned int xint = (unsigned int) x;
                fill((unsigned int)y, xint, xint + 1);
            }
        }
        if( mode == 0 || (*e).delta() < 0)
        {
            x = (*e).x2();
            y = (*e).y2();
            if(x >= 0.0 && x < x_max && y >= 0.0 && y < y_max)
            {
                unsigned int xint = (unsigned int) x;
                fill((unsigned int)y, xint, xint + 1);
            }
        }
    }
}

void Bitmap::render_contour(std::vector<render::RenderEdge>& edges)
{
    for(std::vector<render::RenderEdge>::iterator e = edges.begin(); e != edges.end(); e++)
    {
        // e->y1() == height() - 1 is compatibale with thie condition
        if(e->y1() < double(height() - 0.5) && e->y2() >= -0.5)
        {
            double y = std::max(floor(e->y1() + 0.5), 0.0);
            double y2_bot = e->y2() - 0.5;
            unsigned int yint = (unsigned int) y;
            unsigned int y_max_int =  (unsigned int) std::min(double(height() - 1), std::max(floor(e->y2() + 0.5), 0.0));

            double x = e->pos(y - 0.5);
            double dx = e->pos(y + 0.5) - x;
            double i_slope = (e->y2() - e->y1()) < 1e-6 ? 0.0 : ((double)(e->x2() - e->x1())) / (e->y2() - e->y1());
            unsigned int xint = (unsigned int) std::max(0.0, std::min(double(width() - 1), x) + 0.5);

            if(x < (double) (width () - 0.5) && x >= 0.0)
            {
                fill(yint, xint, xint + 1);
            }

            if(e->x2() > e->x1())
            {
                while(yint <= y_max_int)
                {
                    double x2;
                    if(double(yint) > y2_bot)
                    {
                        x2 = e->x2() + 0.5;
                    }
                    else{
                        x2 = x + dx;
                        dx = i_slope;
                    }

                    unsigned int x2_int;
                    if(x2 >= 0)
                    {
                        if(x2 >= double(width()))
                        {
                            if(x >= double(width() - 1))
                            {
                                break;
                            }
                            x2_int = width() - 1;
                        }
                        else{
                            x2_int = (unsigned int) x2;
                        }

                        if(x2_int <= xint)
                        {
                            fill(yint, xint, xint + 1);
                        }
                        else{
                            fill(yint, xint + 1, x2_int + 1);
                            xint = x2_int;
                        }
                    }
                    else
                    {
                        xint = 0;
                    }
                    x = x2;
                    yint++;
                }
            }
            else
            {
                while(yint <= y_max_int)
                {
                    double x2;
                    if(yint > y2_bot)
                    {
                        x2 = e->x2() - 0.5;
                    }
                    else{
                        x2 = x + dx;
                        dx = i_slope;
                    }

                    unsigned int x2_int;
                    if(x2 < double((width() - 1)))
                    {
                        if(x2 < 0.0)
                        {
                            if(x < 0.0)
                            {
                                break;
                            }
                            x2_int = 0;
                        }
                        else{
                            x2_int = (unsigned int) x2;
                            if(double(x2_int) != x2)
                            {
                                x2_int++;
                            } // right shift the left boundary
                        }
                        if(x2_int >= xint)
                        {
                            fill(yint, xint, xint + 1);
                        }
                        else{
                            fill(yint, x2_int, xint);
                            xint = x2_int;
                        }

                    }
                    else
                    {
                        xint = width();
                    }

                    x = x2;
                    yint++;

                }
            }
        }
    }
}

void Bitmap::render_contour_ortho(std::vector<render::RenderEdge> & edges)
{
    for(std::vector<render::RenderEdge>::iterator e = edges.begin(); e != edges.end(); e++)
    {
        if(!e->is_horizontal())
        {
            double x = e->x1();
            if(e->y1 () < (double)(height() - 0.5) && e->y2() >= -0.5 && x < (double) width() - 0.5 && x >= -0.5)
            {
                unsigned int xint =(unsigned int) std::max(0.0, std::min(double (width() - 1), x)+ 0.5);
                unsigned int yint = (unsigned int) std::max(0.0,floor(e->y1() + 0.5));
                unsigned int yeint = (unsigned int) std::min(double (height() - 1), std::max(0.0, floor(e->y2() + 0.5)));
                while(yint <= yeint)
                {
                    fill(yint, xint, xint + 1);
                    ++yint;
                }
              
            }
        }
        else
        {
            double x1 = e->x1();
            double x2 = e->x2();
            double y = e->y1();
            if(x1 > x2)
            {
                std::swap(x1, x2);
            }

            if ( x1 < width() - 0.5 && x2 >= -0.5 && y  >= -0.5 && y < height() - 0.5)
            {
                unsigned int yint = (unsigned int) std::max(0.0, std::min( double (height() - 1), floor(y + 0.5)));
                unsigned int xint = (unsigned int) std::max(0.0, floor(x1 + 0.5));
                unsigned int xeint = (unsigned int) std::max(0.0, std::min(double (width() - 1), floor(x2 + 0.5)));
                fill(yint, xint, xeint + 1);// complement one pixel for short lines
            }
        }
    }
}

struct PosCompareF
{
    bool operator()(const render::RenderEdge &a, const render::RenderEdge &b) const
    {
     return a.pos() < b.pos();
    }
};

struct X1CompareF
{
    bool operator()(const render::RenderEdge &a, const render::RenderEdge &b) const
    {
        return a.x1() < b.x1();
    }
};

void Bitmap::render_fill(std::vector<render::RenderEdge> &edges)
{
    std::sort(edges.begin(), edges.end());
    double y = std::max(0.0, floor(edges.begin()->y1()));
    std::vector<render::RenderEdge>::iterator done = edges.begin();
    
    while(done != edges.end() && y < height())
    {
        for(; done != edges.end(); done++)
        {
            if(!done->out_of_top(y))
            {
                break;
            }
        }
        std::vector<render::RenderEdge>::iterator todo = done;
        for(; todo != edges.end(); todo++)
        {
            if(todo->out_of_top(y))
            {
                std::swap(*done, *todo);
                done++;
            }
            if(todo->out_of_bottom(y))
            {
                break;
            }
        }
        
        std::vector<render::RenderEdge>::iterator e;
        for(e = done; e != todo; ++e)
        {
            e->set_pos(e->x1() + e->slope() * (y - e->y1()));
        }
        
        PosCompareF f;
        std::sort(done, todo, f);

        int sign = 0;
        bool x1set = false;
        double x1 = 0;
        unsigned int yint = (unsigned int)(y + 0.5);
        
        for(e = done; e != todo; e++)
        {
            if(!e->is_horizontal())
            {
                sign += e->delta();
                if(sign == 0)
                {    
                    unsigned int x1int = 0;
                    if(e->pos() > 0)
                    {
                        if(x1 > 0.0)
                        {
                            x1int = (unsigned int) x1;
                            if(double(x1int) != x1)
                            {
                                x1int++;
                            }
                        }
                        fill(yint, x1int, (unsigned int) std::min(double (width() - 1), e->pos()) + 1);
                    }                    
                    x1set = false;
                }

                else if(!x1set)
                {
                    x1set = true;
                    x1 = e->pos();
                    if(x1 >= width())
                    {
                        break;
                    }
                }
            }
        }
        y += 1.0;
    }
}

void Bitmap::render_fill_ortho(std::vector<render::RenderEdge> &edges)
{
    std::sort(edges.begin(), edges.end());

    double y = std::max(0.0, floor(edges.begin()->y1()));
    std::vector<render::RenderEdge>::iterator done = edges.begin();
    
    while(done != edges.end() && y< height())
    {
        for(; done != edges.end(); done++)
        {
            if(!done->out_of_top(y))
            {
                break;
            }
        }
        std::vector<render::RenderEdge>::iterator todo = done;
        for(; todo != edges.end(); todo++)
        {
            if(todo->out_of_top(y))
            {
                std::swap(*done, *todo);
                done++;
            }
            if(todo->out_of_bottom(y))
            {    
                break;
            }
        }
        std::sort(done, todo, X1CompareF());
        std::vector<render::RenderEdge>::iterator e;
        
        int sign = 0;
        bool x1set = false;
        double x1 = 0;
        unsigned int yint = (unsigned int)(y + 0.5);
        
        for( e = done; e != todo; e++)
        {
            if(!e->is_horizontal())
            {
                sign += e->delta();
                if(sign == 0)
                {
                    if(e->x1() > 0)
                    {
                        unsigned int x1int = 0;
                        if(x1 > 0.0)
                        {
                            x1int = (unsigned int)x1;
                            if(double (x1int) != x1)
                            {
                                x1int++;
                            }
                        }
                        fill(yint, x1int, (unsigned int) std::min(double (width() - 1), e->x1()) + 1);
                    }
                    x1set = false;
                }
                else if(!x1set)
                {
                    x1 = e->x1();
                    x1set =true;
                    if(x1 >= double(width()))
                    {
                        break;
                    }
                }
            }
        }

        y += 1.0;
    }
}


}//namespace lay 
