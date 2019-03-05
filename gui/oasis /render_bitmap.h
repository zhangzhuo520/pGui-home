#ifndef RENDER_BITMAP_H
#define RENDER_BITMAP_H

#include <stdint.h>
#include <math.h>
#include <vector>

namespace render{

const double render_epsilon = 1e-6;

class RenderEdge
{
public: 
    RenderEdge(double x1, double y1, double x2, double y2) : m_d(true), m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2)
    {

        if (point_less(m_x2, m_y2, m_x1, m_y1))
        {
            std::swap(m_x1, m_x2);
            std::swap(m_y1, m_y2);
            m_d = !m_d;
        }
      
        m_horizontal = fabs(m_y2 - m_y1) < render_epsilon;
        m_vertical = fabs(m_x2 - m_x1) < render_epsilon;

        if(m_horizontal)
        {
            m_slope = 0;
        }
        else
        {
            m_slope = (double)(m_x2 - m_x1) / (double)(m_y2 - m_y1);
        }
        m_pos = 0.0;
    }

    double x1() const { return m_x1; }
    double y1() const { return m_y1; }
    double x2() const { return m_x2; }
    double y2() const { return m_y2; }
  
    bool operator< (const RenderEdge &b) const
    {
        return point_less(m_x1, m_y1, b.m_x1, b.m_y1) || (point_equal(m_x1, m_y1, b.m_x1, b.m_y1) && point_less(m_x2, m_y2, b.m_x2, b.m_y2));
    }

    bool operator==(const RenderEdge &b) const
    {
        return point_equal(m_x1, m_y1, b.m_x1, b.m_y1) && point_equal(m_x2, m_y2, b.m_x2, b.m_y2);
    }

    bool operator!= (const RenderEdge &b) const
    {
        return !operator== (b);
    }

    int delta() const
    {
        return m_d ? 1 : -1;
    }
    double slope() const
    {
        return m_slope;
    }
  
    bool is_horizontal() const
    {
        return m_horizontal;
    }

    bool is_vertical() const
    {
        return m_vertical;
    }
  
    int is_swap() const 
    {
        return m_d ? 1 : -1 ;
    }
  
    double pos() const
    {
        return m_pos;
    }    
  
    void set_pos(double p)
    {
        m_pos = p;
    }

    double pos(double y) const
    {
        if(y > y2())
        {
            return x2();
        }
        else if(y < y1())
        {
            return x1();
        }
        return m_slope * (y - y1()) + x1();
    }

    bool out_of_top(double y)
    {
        return y > m_y2;
    }
  
    bool out_of_bottom(double y)
    {
        return y <= m_y1;
    }
private:
    bool point_less(double xx1, double yy1, double xx2, double yy2) const
    {
        return yy1 < yy2 || (yy1 == yy2 && xx1 < xx2);
    }
    bool point_equal(double xx1, double yy1, double xx2, double yy2) const
    {
        return xx1 == xx2 && yy1 == yy2;
    }
private:
    double m_pos;
    bool m_d, m_horizontal,m_vertical;
    double m_slope;

    double m_x1;
    double m_y1;
    double m_x2;
    double m_y2;
};

class Bitmap
{
public:
    Bitmap ();
    Bitmap (unsigned int width, unsigned int height, double resolution);
    Bitmap (const Bitmap &bm);
    Bitmap& operator= (const Bitmap &bm);
    virtual ~Bitmap ();

    const uint32_t* empty_scanline() const
    {
        return m_empty_scanline;
    }

    const uint32_t* scanline(unsigned int y) const
    {
        if (y >= m_scanlines.size() || m_scanlines[y] == 0)
        {
            return m_empty_scanline;
        }
        return m_scanlines[y];
    }

    bool is_empty_scanline(unsigned int y) const { return m_scanlines.empty() || m_scanlines[y] == 0; }

    uint32_t * scanline(unsigned int n);

    double resolution() const { return m_resolution; }

    unsigned int width() const { return m_width; }
    
    unsigned int height() const { return m_height; }
    
    void fill(unsigned int y, unsigned int x1, unsigned int x2);

    bool empty() const { return m_first_line > m_last_line; }

    unsigned int first_scanline() const { return m_first_line; }

    unsigned int last_scanline() const { return m_last_line; }
    
    void render_fill(std::vector<render::RenderEdge> &edges);
    
    void render_fill_ortho(std::vector<render::RenderEdge> &edges);

    void render_vertices(std::vector<render::RenderEdge> &edges, int mode);

    void render_contour(std::vector<render::RenderEdge>& edges);

    void render_contour_ortho(std::vector<render::RenderEdge> &edges);

    void clear();
    
    virtual void pixel(unsigned int x, unsigned int y)
    {
        fill(y, x, x + 1);
    }

private:
    unsigned int m_width;
    unsigned int m_height;
    double m_resolution;
    std::vector<uint32_t *> m_scanlines;
    std::vector<uint32_t *> m_free_scanlines;
    uint32_t* m_empty_scanline;
    unsigned int m_first_line;
    unsigned int m_last_line;

    void init(unsigned int width, unsigned int height);
    void clean();

    void fill_brush(int y, int x, const uint32_t *p, unsigned int stride, unsigned int n);
};

}
#endif


