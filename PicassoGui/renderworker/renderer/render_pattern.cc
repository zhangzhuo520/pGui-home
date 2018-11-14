#include "render_pattern.h"
#include <ctype.h>
#include <algorithm>

namespace render
{
static const char *pattern_strings[] = {

    //0:solid
    "solid",
    "*",

    //1:holow
    "hollow",
    ".",

    //2:dotted
    "dotted",
    "*.\n"
    ".*",

    //3:coarsly dotted
    "coarsly dotted",
    "*...\n"
    "....\n"
    "...*\n"
    "....",

    //4:left-hatched
    "left-hacthed",
    "*...\n"
    ".*..\n"
    "..*.\n"
    "...*",

    //5:lightly left-hatched
    "lightly left-hatched",
    "*.......\n"
    ".*......\n"
    "..*.....\n"
    "...*....\n"
    "....*...\n"
    ".....*..\n"
    "......*.\n"
    ".......*"

};


PatternInfo::PatternInfo() : m_width(1), m_height(1), m_order_index(0)
{
    m_stride = 1;
    for(size_t i = 0; i < (sizeof(m_pattern)/sizeof(m_pattern[0])); i++)
    {
        m_pattern[i] = &m_buffer[0];
    }
    memset(m_buffer, 0xff, sizeof(m_buffer));
}
   
PatternInfo::PatternInfo(const PatternInfo &d) : m_width(d.m_width), m_height(d.m_height), m_order_index(d.m_order_index)
{
    this->operator=(d);
}

PatternInfo& PatternInfo::operator = (const PatternInfo &d)
{
    if(&d !=this)
    {
        m_name = d.m_name;
        m_stride = d.m_stride;
        m_width = d.m_width;
        m_height = d.m_height;
        m_order_index = d.m_order_index;
        for(size_t i = 0; i< (sizeof(m_pattern)/sizeof(m_pattern[0])); i++)
        {
            m_pattern[i] = &m_buffer[0] + (d.m_pattern[i] - d.m_buffer);
        }
        memcpy(m_buffer, d.m_buffer, sizeof(m_buffer));
    }
    return *this;
}

bool PatternInfo::same(const PatternInfo &d) const
{
    if (&d == this)
    {
        return true;
    }
    if (m_width != d.m_width || m_height != d.m_height)
    {
        return false;
    }

    if (m_stride != d.m_stride)
    {
        return false;
    }
    unsigned int n = m_stride * (sizeof(m_pattern) / sizeof(m_pattern[0]));
    for (unsigned int i = 0; i < n; i++)
    {
        if (m_buffer[i] != d.m_buffer[i])
        {
            return false;
        }
    }

    return true;
}

bool PatternInfo::operator== (const PatternInfo &d) const
{
    return same(d) && m_name == d.m_name && m_order_index == d.m_order_index;
}

bool PatternInfo::less(const PatternInfo &d) const
{
    if(m_width != d.m_width)
    {
        return m_width < d.m_width;
    }
    if(m_height != d.m_height)
    {
        return m_height < d.m_height;
    }
    unsigned int sum = m_stride *(sizeof(m_pattern) / sizeof(m_pattern[0]));

    for(unsigned int i = 0 ; i < sum ; i++)
    {
        if(m_buffer[i] < d.m_buffer[i])
        {
            return true;
        }
        else if (m_buffer[i] > d.m_buffer[i])
        {
            return false;
        }
    }
    return false;
}

bool PatternInfo::operator<(const PatternInfo &d) const
{
    if(!same(d))
    {
        return false;
    }
    if(m_name != d.m_name)
    {
        return m_name < d.m_name;
    }
    return m_order_index < d.m_order_index;
}

QBitmap PatternInfo::get_bitmap(int width, int height) const
{
    if(height < 0)
    {
        height = 36;         
    }
    if(width < 0)
    {
        width = 34;
    }  
            
    const uint32_t* const * p = pattern(); 
    unsigned int segs = (width + 7)/8;
    unsigned char * data = new unsigned char[segs * height];
    memset(data, 0, size_t (segs * height));
        
    for(unsigned int i = 1; i < (unsigned int) (height - 1); i++)
    {
        for(unsigned int j = 0; j < segs ; j++)
        {
            data[i * segs + j] = 0xff;
        }
    }
    
    for(unsigned int i = 0; i < (unsigned int) (height - 4);++i)
    {
        uint32_t w = *( p[(height - 5 - i) % m_height]);
        for(unsigned int j = 0 ; j < (unsigned int) (width - 2); ++j)
        {
            if(!(w &(1 << (j % m_width))))
            {
                 data[segs *(i + 2) + (j + 1) / 8] &= ~(1<< ((j + 1) % 8));
            }
        }
    }

    QBitmap bitmap(QBitmap::fromData(QSize(width, height), data, QImage::Format_MonoLSB));
    delete[] data;
    return bitmap;
     
}

void PatternInfo::set_pattern(const uint32_t *pt, unsigned int width, unsigned int height)
{
    if(width == 0 || height == 0)
    {
         uint32_t zero = 0;
         set_pattern(&zero, 1,1);
         return;
    }

    memset(m_buffer, 0 , sizeof(m_buffer));
    m_width = width > 32 ? 32 : width;
    m_height = height > 32 ? 32 : height;

    m_stride = 1;
    while( (m_stride * 32) % width != 0)
    {
        ++m_stride;
    }

    uint32_t *row_addr = &m_buffer[0];
    for(unsigned int i = 0; i < sizeof(m_pattern) / sizeof(m_pattern[0]); ++i)
    {
        m_pattern[i] = row_addr;

        uint32_t data = pt[i % height];
        uint32_t temp = data;

        unsigned int data_iter = 0;
        for(unsigned int j = 0; j < m_stride; j++)
        {
            uint32_t result= 0;
            for(uint32_t op = 1; op != 0; op <<= 1)
            {
                if((temp & 1) != 0)
                {
                    result |= op;
                }
                temp >>= 1;
                data_iter++;
                if(data_iter == width)
                {
                    data_iter = 0;
                    temp = data;
                }
            }
            *row_addr = result;
            row_addr++;
        }
    }
}


std::string PatternInfo::to_string() const
{
    std::string result;
    for(unsigned int i = 0 ; i< m_height; i ++)
    {
        for(unsigned int j = 0; j < m_width; j++)
        {
            if( (*(m_pattern[m_height - 1 - i] ) &(1 << j)) != 0)
            {
                result += "*";
            }
            else
            {
                result += ".";
            }
                
        }
    }
    return result;
}

static const char *uint_from_string(const char *s, uint32_t &word, unsigned int &width)
{
    while(*s && isspace(*s))
    {
        ++s;
    }

    unsigned int op = 1;
    unsigned int col = 0;
    while(*s && !isspace(*s))
    {
        if(*s++ == '*')
        {
            word |= op;
        }
        op <<= 1;
        col++;
    }
    width = std::max(width, col);
    return s;
}


void PatternInfo::from_string(const std::string &str)
{
    unsigned int width = 0;
    unsigned int height = 0;
    uint32_t buffer[32];

    for (unsigned int i = 0; i < 32; i++)
    {
        buffer[i] = 0;
    }

    const char *s = str.c_str();

    while (*s && height < 32)
    {
        while (*s && isspace(*s))
        {
            ++s;
        }
        if (*s)
        {
            s = uint_from_string(s, buffer[height], width);
            height++;
        }
    }

    std::reverse(&buffer[0], &buffer[height]);
    set_pattern(buffer, width, height);
}

Pattern::Pattern() : QObject()
{
    for(unsigned int i = 0; i < (sizeof(pattern_strings)/ sizeof(pattern_strings[0])); i+=2)
    {
        m_pattern.push_back(PatternInfo());
        std::string s_name = pattern_strings[i];
        std::string s_content = pattern_strings[i + 1];
        m_pattern.back().set_name(s_name);
        m_pattern.back().from_string(s_content);
    }
}

Pattern::~Pattern()
{
}

Pattern::Pattern(const Pattern &p) : QObject()
{
    m_pattern = p.m_pattern;
}

Pattern& Pattern::operator = (const Pattern &d)
{
    if(&d != this)
    {
        unsigned int i;
        for(i = 0; i < d.count(); i++)
        {
            replace_pattern(i, d.begin()[i]);
        }
        for(; i < count(); i++)
        {
            replace_pattern(i, PatternInfo());
        }
    }

    return *this;
}

void Pattern::replace_pattern(unsigned int i, const PatternInfo &p)
{
    bool changed = false;
    while(i >= count())
    {
        m_pattern.push_back(PatternInfo());
        changed = true;
    }
    if(m_pattern[i] != p)
    {
        m_pattern[i] = p;
        changed = true;
    }
    if(changed)
    {
        ;
    }
}

namespace{
struct display_order
{
    bool operator ()(render::Pattern::iterator a, render::Pattern::iterator b)
    {
        return a->order_index() < b->order_index();
    }
};
}

void Pattern::reorder()
{
    std::vector<render::Pattern::iterator> iters;
    for (render::Pattern::iterator i = begin(); i != end(); ++i)
    {
        iters.push_back(i);
    }

    std::sort(iters.begin(), iters.end(), display_order());
    unsigned int oi = 1;
    for (std::vector<render::Pattern::iterator>::const_iterator i = iters.begin(); i != iters.end(); i++)
    {
        if( (*i)->order_index() > 0 )
        {
            render::PatternInfo p(**i);
            p.set_order_index(oi++);
            replace_pattern(std::distance(begin(), *i), p);
        }
    }
}

QBitmap Pattern::get_bitmap(unsigned int i, int width, int height) const
{
    if(i < count())
    {
        return m_pattern[i].get_bitmap(width, height);
    }
    else
    {
        return m_pattern[1].get_bitmap(width, height);
    }
}

const PatternInfo & Pattern::pattern(unsigned int i) const
{
    if (i< count())
    {
        return m_pattern[i];
    }
    else
    {
        static PatternInfo empty;
        return empty;
    }
}

const Pattern& Pattern::default_pattern()
{
    static Pattern emptyPattern;
    return emptyPattern;
}
}
