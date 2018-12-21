#ifndef RENDER_LINESTYLE_H
#define RENDER_LINESTYLE_H


#include <QObject>
#include <QBitmap>

#include <stdint.h>
#include <vector>
#include <string>
#include <map>


namespace render
{

class LineStyleInfo
{
public:
    LineStyleInfo();
    
    LineStyleInfo(const LineStyleInfo &li);
    
    LineStyleInfo& operator=(const LineStyleInfo &li);

    bool same_line_info(const LineStyleInfo &li) const;

    bool less_line_info(const LineStyleInfo &li) const;

    bool operator==(const LineStyleInfo &li) const;

    bool operator<(const LineStyleInfo &li) const;

    bool operator!=(const LineStyleInfo &li) const
    {
        return ! operator==(li);
    }

    bool is_bit_set(unsigned int n ) const;

    const std::string& name() const
    {
        return m_name;
    }

    void set_name(const std::string& s)
    {
        m_name = s;
    }

    unsigned int order_index() const
    {
        return m_order_index;
    }    

    void set_order_index(unsigned int oi)
    {
        m_order_index = oi;
    }    

    const uint32_t *pattern() const
    {
        return m_pattern;
    }

    QBitmap get_bitmap( int width = -1, int height = -1) const;

    void set_pattern(uint32_t data, unsigned int w);

    unsigned int stride() const
    {
        return m_stride;
    }

    unsigned int width() const 
    {
        return m_width;
    }

    void from_string(const std::string &s);

    std::string to_string() const;

private:
    uint32_t m_pattern[32];
    unsigned int m_width;
    unsigned int m_stride;
    unsigned int m_order_index;
    std::string m_name;
};



class LineStyle : public QObject
{
Q_OBJECT
public:

    typedef std::vector<render::LineStyleInfo> line_style_vector;
    typedef line_style_vector::const_iterator iterator;

    LineStyle();
    ~LineStyle();
    LineStyle(const LineStyle &l);
    LineStyle& operator=(const LineStyle &l);

    bool operator==(const LineStyle &l) const
    {
        return m_style == l.m_style;
    }

    bool operator!=(const LineStyle &l) const
    {
        return m_style != l.m_style;
    }

    const LineStyleInfo &style (unsigned int style) const;

    void replace_style( unsigned int i, const LineStyleInfo &li);

    void reorder();

    unsigned int count() const
    {
        return (unsigned int) m_style.size();
    }
    iterator begin() const
    {
        return m_style.begin();
    }

    iterator end() const
    {
        return m_style.end();
    }

    static const LineStyle& default_style();
signals:
    void changed();
private:
    line_style_vector m_style;
};

}
#endif
