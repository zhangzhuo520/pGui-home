#ifndef RENDER_PATTERN_H
#define RENDER_PATTERN_H

#include <QBitmap>
#include <QObject>
#include <stdint.h>
#include <vector>
#include <string>
#include <map>

namespace render{

class PatternInfo
{
public:
    PatternInfo();

    PatternInfo(const PatternInfo &d);

    PatternInfo& operator=(const PatternInfo &d);
    bool operator<(const PatternInfo &d) const;
    bool operator==(const PatternInfo &d) const;
    bool operator!=(const PatternInfo &d) const
    {
        return !operator==(d);
    }

    bool less(const PatternInfo &d) const;


    bool same(const PatternInfo &d) const;

    const std::string& name() const
    {
        return m_name;
    }

    void set_name(std::string& name)
    {
        m_name = name;
    }

    QBitmap get_bitmap(int width =-1, int height= -1) const;

    const uint32_t* const * pattern() const
    {
        return &(m_pattern[0]);
    }


    void set_pattern(const uint32_t *pattern, unsigned int w, unsigned int h);

    unsigned int stride() const
    {
        return m_stride;
    }

    unsigned int order_index() const
    {
        return m_order_index;
    }

    void set_order_index(unsigned int oi)
    {
        m_order_index = oi;
    }

    void from_string(const std::string&  s);

    std::string to_string() const;
    

    unsigned int width() const
    {
        return m_width;
    }
    
    unsigned int height() const
    {
        return m_height;
    }
    
private:
    std::string m_name;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_stride;
    unsigned int m_order_index;
    uint32_t* (m_pattern[64]);
    uint32_t m_buffer[64 * 32];
};

class Pattern : public QObject
{
Q_OBJECT
public:
    typedef std::vector<PatternInfo> pattern_vector;
    typedef pattern_vector::const_iterator iterator;

    Pattern();
    Pattern(const Pattern &p);
    Pattern& operator= (const Pattern &p);
    ~Pattern();

    bool operator==(const Pattern &p) const
    {
        return m_pattern == p.m_pattern;
    }

    bool operator!=(const Pattern &p) const
    {
        return m_pattern != p.m_pattern;
    }

    QBitmap get_bitmap(unsigned int i, int width = -1,  int height = -1) const;

    const PatternInfo &pattern(unsigned int i) const;

    unsigned int count() const
    {
        return (unsigned int) m_pattern.size();
    }

    iterator begin() const
    {
        return m_pattern.begin();
    }

    iterator end() const
    {
        return m_pattern.end();
    }

    void reorder();
    void replace_pattern(unsigned int i, const PatternInfo &p);
    static const Pattern & default_pattern();
private:
    pattern_vector m_pattern;
};
}
#endif

