#include "render_line_style.h"
#include <ctype.h>

#include <algorithm>

namespace render{

static const char* style_strings[]={

    // 0: solid
    "solid",
    "",
   
    // 1: long dashed
    "long dashed",
    "*****..*****",
   
    // 2: dashed
    "dashed",
    "**..**..**",
	
	// 3: short dash-dotted
    "short dash-dotted",
    "**.*.*",
	
    // 4: dotted
    "dotted",
    "*.",

    // 5: dash-dotted 
    "dash-dotted",
    "***..**..***",

    // 6: short dashed
    "short dashed",
    "*..*",

    // 7: dash-double-dotted
    "dash-double-dotted",
    "***..*.*..**"
};

LineStyleInfo::LineStyleInfo() : m_width(0), m_order_index(0)
{
    m_stride = 1;
    memset(m_pattern, 0xff, sizeof(m_pattern));
}

LineStyleInfo::LineStyleInfo(const LineStyleInfo &li) : m_width(li.m_width), m_order_index(li.m_order_index), m_name(li.m_name)
{
    operator=(li);
}

LineStyleInfo &
LineStyleInfo::operator=(const LineStyleInfo &li)
{
    if(this != &li)
    {
        m_width = li.m_width;
        m_name = li.m_name;
        m_order_index = li.m_order_index;
        m_stride = li.m_stride;
        memset(m_pattern, 0xff, sizeof(m_pattern));
        memcpy(m_pattern, li.m_pattern, sizeof(m_pattern));
    }
    return *this;
}

bool LineStyleInfo::same_line_info(const LineStyleInfo &li) const
{
    if(m_width != li.m_width)
    {
        return false;
    }
    
    for(unsigned int i = 0 ; i < m_stride; i++)
    {
        if(m_pattern[i] != li.m_pattern[i])
        {
            return false;
        }
    }
    return true;
}


bool LineStyleInfo::less_line_info(const LineStyleInfo &li) const 
{
    if(m_width != li.m_width)
    {
        return m_width < li.m_width;
    }
    
    for (unsigned int i = 0; i < m_stride; i++)
    {
        if(m_pattern[i] < li.m_pattern[i])
        {
            return true;
        }
        else if(m_pattern[i] > li.m_pattern[i])
        {
            return false;
        }
    }
    return false;
}


bool  LineStyleInfo::operator== (const LineStyleInfo &li) const
{
    return same_line_info(li) && m_name == li.m_name && m_order_index == li.m_order_index;
}


bool LineStyleInfo::operator< (const LineStyleInfo &li) const
{
    if(!same_line_info(li))
    {
        return less_line_info(li);
    }
    
    if(m_name != li.m_name)
    {
        return m_name < li.m_name;
    }
    return m_order_index < li.m_order_index;
}


bool LineStyleInfo::is_bit_set(unsigned int n) const
{
    return ((pattern()[(n / 32) % m_stride]) & (1 << (n % 32))) != 0;
}

QBitmap LineStyleInfo::get_bitmap(int width, int height) const
{
    if(height < 0)
    {
        height = 5;
    }
    
    if(width < 0)
    {
        width = 34;
    }
    
    unsigned int stride = (width + 7) / 8;
    unsigned char * data = new unsigned char [stride * height];
    memset(data, 0x00, size_t(stride * height));
    
    // at vetrical directrion we draw a line style on the left bound and right bound
    //
    for(unsigned int i = 0; i < (unsigned int) (height -2) ; ++i)
    {
        if(is_bit_set(i))
        {
            data[(height - 2 - i) * stride] |= 0x01;
            data[(height - 2 - i) * stride + (width - 1) /8]|= (1 << (width - 1)/ 8);
        }
    }    

    for(unsigned int i = 1; i < (unsigned int)(width - 1); ++i)
    {
        if(is_bit_set(i))
        {
            data[stride + i / 8] |= (1 << (i % 8));
            data[(height - 2) * stride + i / 8] |= (1 << (i % 8));
        }
    }
    
    QBitmap bitmap(QBitmap::fromData(QSize (width, height), data, QImage::Format_MonoLSB));
    delete[]data;
    return bitmap;
}


void LineStyleInfo::set_pattern(uint32_t data, unsigned int width)
{
    memset(m_pattern, 0xff, sizeof(m_pattern));
    
    if(width > 32)
    {
        width = 32;
    }
    
    m_width = width;

    if( width == 0)
    {
        m_pattern[0] = 0xffffffff;
        m_stride = 1;
        return;
    }
        
    m_stride = 1;
    while( (m_stride * 32) % width != 0)
    {
        m_stride ++;
    }

    uint32_t * buffer_start = m_pattern;
    uint32_t temp = data;

    uint32_t style_sum = 0;
    for(unsigned int i = 0 ; i < m_stride ; i++)
    {
        uint32_t result = 0;
        for(uint32_t op = 1; op != 0 ; op <<= 1)
        {
            if( (temp & 1) !=0)
            {
                result |= op;
            }
            temp >>= 1;
            if ( ++style_sum == width)
            {
                style_sum = 0;
                temp = data;
            }
        }
        *buffer_start++ = result;
    }
}


std::string 
LineStyleInfo::to_string()  const
{
    std::string res;
    for(unsigned int i = 0; i < m_width; i++)
    {
        if( (m_pattern[0] & (1 << i)) !=0)
        {
            res += "*";
        }
        else
        {
            res += ".";
        }
    }
    return res;
}


static const char* uint_from_string(const char *s, uint32_t & data, unsigned int& width)
{
    while(*s && isspace(*s))
    {
        s++;
    }

    data  = 0;
    
    unsigned int op = 1;
    unsigned int size = 0;

    while(*s && !isspace(*s))
    {
        if(*s++ == '*')
        {
            data |= op;
        }
        op <<= 1;
        size++;
    }

    width = std::max(width, size);
    return s;

}

void LineStyleInfo::from_string(const std::string & style)
{
    unsigned int  w = 0;
    uint32_t data = 0;
    const char *s = style.c_str();
    uint_from_string(s, data, w);
    set_pattern(data, w);
} 

//=============================================================================//

LineStyle::LineStyle():QObject()
{
    for(unsigned int i = 0; i < sizeof(style_strings) / sizeof(style_strings[0]) ; i+=2)
    {
        m_style.push_back(LineStyleInfo());
        m_style.back().set_name(style_strings[i]);
        m_style.back().from_string(style_strings[i + 1]);
    }
} 


LineStyle::~LineStyle()
{

}


LineStyle::LineStyle(const LineStyle & ls) : QObject()
{
    m_style = ls.m_style;
}


LineStyle&
LineStyle::operator=(const LineStyle &ls)
{
    if(this != &ls)
    {
        unsigned int i;
        for(i = 0; i < ls.count() ; i++)
        {
            replace_style(i, ls.begin()[i]);
        }
        
        for(; i < count(); i++)
        {
            replace_style(i, LineStyleInfo());
        }
    }
    return *this;
}


const LineStyleInfo&  LineStyle::style(unsigned int style_index) const
{
    if(style_index < count())
    {
        return m_style[style_index];
    }
    
    static LineStyleInfo empty;
    return empty;
    
}

void LineStyle::replace_style(unsigned int i, const LineStyleInfo& ls)
{
    bool change = false;
    
    while(i >=count())
    {
        m_style.push_back(LineStyleInfo());
        change = true;
    }

    if(m_style[i] != ls)
    {
        m_style[i] = ls;
        change = true;
    }
    
    if(change)
    {
        //emit changed();
    }
}


namespace {
    struct display_order
    {
        bool operator()(render::LineStyle::iterator a, render::LineStyle::iterator b)
        {
            return a->order_index() < b->order_index();
        }
    };
}

void LineStyle::reorder()
{
    std::vector<render::LineStyle::iterator> iter;
    for(render::LineStyle::iterator i = begin(); i != end() ; i++)
    {
        iter.push_back(i);
    }

    std::sort(iter.begin(), iter.end(), display_order());
    
    unsigned int order_index = 1;
    for(std::vector<render::LineStyle::iterator>::const_iterator i = iter.begin(); i != iter.end(); i ++)
    {
        if( (*i)->order_index() > 0)
        {
            render::LineStyleInfo p(**i);
            p.set_order_index(order_index++);
            replace_style(std::distance(begin(), *i), p);
        }
    }
    
}

QBitmap LineStyle::get_bitmap(unsigned int i, int width, int height) const
{
    if(i < count())
    {
        return m_style[i].get_bitmap(width, height);
    }
    else
    {
        return m_style[1].get_bitmap(width, height);
    }
}

const LineStyle& LineStyle::default_style() 
{
    static LineStyle empty;
    return empty;
}


}//namespace lay
