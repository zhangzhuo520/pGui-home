#ifndef OASIS_TYPE_H
#define OASIS_TYPE_H
#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include "logger.h"

#define oas_assert(x) do{ if(!(x)){ oasis::oasis_error(#x, __FILE__, __LINE__); }}while(0)

namespace oasis
{

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

void oasis_error(const std::string& expr, const std::string& file, int line);

inline int64 uint_to_int(const uint64& value)
{
    if((value & 1) != 0)
    {
        return -int64(value >> 1);
    }
    else
    {
        return int64(value >> 1);
    }
}

inline uint64 int_to_uint(const int64& value)
{
    if(value < 0)
    {
        return ((uint64)(-value) << 1) | 1;
    }
    else
    {
        return (uint64)value << 1;
    }
}

template<typename C>
struct type_traits
{
};

template<>
struct type_traits<int64>
{
    template<typename V>
    static int64 rounded(V v) { return (int64)rint(v); }
};

template<>
struct type_traits<float64>
{
    template<typename V>
    static float64 rounded(V v) { return (float64)v;}
};

template<typename C>
class TemplatePoint
{
    template<typename D> friend TemplatePoint<D> operator-(const TemplatePoint<D>& a, const TemplatePoint<D>& b);
    template<typename D> friend TemplatePoint<D> operator+(const TemplatePoint<D>& a, const TemplatePoint<D>& b);
public:
    TemplatePoint() : m_x(0), m_y(0) {}

    TemplatePoint(const C& x, const C& y) : m_x(x), m_y(y) {}

    template<typename T>
    TemplatePoint(const TemplatePoint<T>& rh) :
        m_x(type_traits<C>::rounded(rh.x())),
        m_y(type_traits<C>::rounded(rh.y())) { }

    C x() const { return m_x; }
    C y() const { return m_y; }

    void set_x(const C& x) { m_x = x; }
    void set_y(const C& y) { m_y = y; }

    TemplatePoint<C> operator-() const
    {
        return TemplatePoint<C>(-m_x, -m_y);
    }

    bool operator==(const TemplatePoint<C>& p) const
    {
        return m_x == p.m_x && m_y == p.m_y;
    }

    bool operator!=(const TemplatePoint<C>& p) const
    {
        return !operator==(p);
    }

    bool operator<(const TemplatePoint<C>& p) const
    {
        return m_x < p.m_x || (m_x == p.m_x && m_y < p.m_y);
    }

private:
    C m_x;
    C m_y;
};

template<typename C>
inline TemplatePoint<C> operator-(const TemplatePoint<C>& a, const TemplatePoint<C>& b)
{
    return TemplatePoint<C>(a.x() - b.x(), a.y() - b.y());
}

template<typename C>
inline TemplatePoint<C> operator+(const TemplatePoint<C>& a, const TemplatePoint<C>& b)
{
    return TemplatePoint<C>(a.x() + b.x(), a.y() + b.y());
}

typedef TemplatePoint<int64> Point;
typedef TemplatePoint<float64> PointF;

class OasisTrans
{
public:
    enum RotationType {ROT0 = 0, ROT90 = 1, ROT180 = 2, ROT270 = 3};

    OasisTrans() : m_mag(1.0), m_sin(0.0), m_cos(1.0), m_rot(ROT0), m_ismag(false), m_ismirror(false), m_iscomplex(false) {}
    template<typename C>
    OasisTrans(bool mirror, uint8 rot, const TemplatePoint<C>& disp) :
        m_disp(disp),
        m_mag(1.0),
        m_sin(0.0),
        m_cos(1.0),
        m_rot(rot),
        m_ismag(false),
        m_ismirror(mirror),
        m_iscomplex(false) {}
    template<typename C>
    OasisTrans(bool mirror, float64 angle, float64 mag, const TemplatePoint<C>& disp) :
        m_disp(disp),
        m_mag(mag),
        m_ismag(false),
        m_ismirror(mirror),
        m_iscomplex(true)
    {
        angle *= M_PI / 180.0;
        m_sin = sin(angle);
        m_cos = cos(angle);
        if(fabs(mag - 1.0) > 0.001) m_ismag = true;
    }

    bool is_complex() const { return m_iscomplex; }
    Point disp() const { return m_disp; }
    PointF dispF() const { return m_disp; }
    void set_disp(PointF p) { m_disp = p; }
    float64 angle() const
    {
        if(is_complex())
        {
            float64 a = atan2 (m_sin, m_cos) * (180.0 / M_PI);
            if (a < -0.001)
            {
                a += 360.0;
            }
            else if (a <= 0.001)
            {
                a = 0.0;
            }
            return a;
        }
        else
        {
            return m_rot * 90.0;
        }
    }
    uint8 rotation() const { oas_assert(!is_complex()); return m_rot; }
    bool is_mirror() const { return m_ismirror; }
    float64 mag() const { return m_ismag ? m_mag : 1.0; }
    bool is_mag() const { return m_ismag; }
    bool is_rot() const { oas_assert(!is_complex()); return m_rot > 0; }

    void invert()
    {
        if(m_iscomplex)
        {
            complex_invert();
        }
        else
        {
            simple_invert();
        }
    }

    OasisTrans inverted() const
    {
        OasisTrans t(*this);
        t.invert();
        return t;
    }

    template<typename T>
    void shift(const TemplatePoint<T>& p)
    {
        m_disp.set_x(m_disp.x() + p.x());
        m_disp.set_y(m_disp.y() + p.y());
    }

    template<typename T>
    OasisTrans shifted(const TemplatePoint<T>& p) const
    {
        OasisTrans t(*this);
        t.shift(p);
        return t;
    }

    template<typename T>
    TemplatePoint<T> trans(const TemplatePoint<T>& p) const
    {
        if(m_iscomplex)
        {
            return complex_trans(p);
        }
        else
        {
            return simple_trans(p);
        }
    }

    template<typename T>
    TemplatePoint<float64> transF(const TemplatePoint<T>& p) const
    {
        TemplatePoint<float64> pf(p);
        return trans(pf);
    }

    OasisTrans& operator*=(const OasisTrans& rhs)
    {
        if(!m_iscomplex && !rhs.m_iscomplex)
        {
            simple_combine(rhs);
        }
        else
        {
            complex_combine(rhs);
        }
        return *this;
    }

    OasisTrans operator*(const OasisTrans& rhs) const
    {
        OasisTrans result(*this);
        result *= rhs;
        return result;
    }

private:

    void reset_disp()
    {
        m_disp = PointF();
    }

    void simple_combine(const OasisTrans& rhs)
    {
        m_disp = simple_trans(rhs.m_disp);
        fixpoint_combine(rhs);
    }

    void fixpoint_combine(const OasisTrans& rhs)
    {
        if(!m_ismirror)
        {
            m_rot = (m_rot + rhs.m_rot) & 3;
        }
        else
        {
            m_rot = (m_rot + (4 - rhs.m_rot)) & 3;
        }
        m_ismirror = (m_ismirror ^ rhs.m_ismirror);
    }

    void complex_combine(const OasisTrans& rhs)
    {
        OasisTrans r(rhs);
        to_complex();
        r.to_complex();

        float64 sign = m_ismirror ? -1.0 : 1.0;
        m_disp = complex_trans(r.m_disp);
        m_mag *= r.m_mag;
        if(fabs(m_mag - 1.0) > 0.001) m_ismag = true;
        float64 c = m_cos * r.m_cos - sign * m_sin * r.m_sin;
        float64 s = m_sin * r.m_cos + sign * m_cos * r.m_sin;
        m_cos = c;
        m_sin = s;
        m_ismirror = (m_ismirror ^ r.m_ismirror);
    }

    void fixpoint_invert()
    {
        if(!m_ismirror)
        {
            m_rot = (4 - m_rot) & 3;
        }
    }

    void simple_invert()
    {
        fixpoint_invert();
        PointF p = m_disp;
        reset_disp();
        m_disp = -simple_trans(p);
    }

    void complex_invert()
    {
        m_mag = 1.0 / m_mag;
        m_sin = -m_sin * (m_ismirror ? -1.0 : 1.0);
        PointF u(-m_disp.x(), -m_disp.y());
        reset_disp();
        m_disp = complex_trans(u);
    }

    template<typename T>
    TemplatePoint<T> fixpoint_trans(const TemplatePoint<T>& p) const
    {
        T x = p.x();
        T y = m_ismirror ? -p.y() : p.y();
        switch (m_rot)
        {
            case 0:
                return TemplatePoint<T>(x, y);
            case 1:
                return TemplatePoint<T>(-y, x);
            case 2:
                return TemplatePoint<T>(-x, -y);
            case 3:
                return TemplatePoint<T>(y, -x);
        }
        oas_assert(false);
        return TemplatePoint<T>(x, y);
    }

    template<typename T>
    TemplatePoint<T> simple_trans(const TemplatePoint<T>& p) const
    {
        return fixpoint_trans(p) + TemplatePoint<T>(m_disp);
    }

    template<typename T>
    TemplatePoint<T> complex_trans(const TemplatePoint<T> &p) const
    {
        PointF mp (m_cos * p.x() * m_mag - m_sin * p.y() * m_mag * (m_ismirror ? -1 : 1),
                        m_sin * p.x() * m_mag + m_cos * p.y() * m_mag * (m_ismirror ? -1 : 1));
        return TemplatePoint<T>(mp + m_disp);
    }

    void to_complex()
    {
        if(!m_iscomplex)
        {
            switch(m_rot)
            {
                case 0:
                    m_cos = 1.0;
                    m_sin = 0.0;
                    break;
                case 1:
                    m_cos = 0.0;
                    m_sin = 1.0;
                    break;
                case 2:
                    m_cos = -1.0;
                    m_sin = 0.0;
                    break;
                case 3:
                    m_cos = 0.0;
                    m_sin = -1.0;
                    break;
                default:
                    oas_assert(false);
            }
            m_iscomplex = true;
        }
    }

private:
    PointF m_disp;
    float64 m_mag;
    float64 m_sin;
    float64 m_cos;
    uint8 m_rot;
    bool m_ismag;
    bool m_ismirror;
    bool m_iscomplex;
};

template<typename C>
class EdgeTemplate
{
public:
    EdgeTemplate() {}
    template<typename T>
    EdgeTemplate(const TemplatePoint<T>& p1, const TemplatePoint<T>& p2) : m_p1(p1), m_p2(p2) {}

    const TemplatePoint<C>& p1() const { return m_p1; }
    const TemplatePoint<C>& p2() const { return m_p2; }

    template<typename T>
    void set_p1(const TemplatePoint<T>& p) { m_p1 = p; }

    template<typename T>
    void set_p2(const TemplatePoint<T>& p) { m_p2 = p; }
private:
    TemplatePoint<C> m_p1;
    TemplatePoint<C> m_p2;
};

typedef EdgeTemplate<int64> Edge;
typedef EdgeTemplate<float64> EdgeF;

template<typename C>
class TemplateBox
{
public:
    TemplateBox() : m_left(1), m_bottom(1), m_right(-1), m_top(-1) {}
    template<typename T1, typename T2, typename T3, typename T4>
    TemplateBox(const T1& left, const T2& bottom, const T3& right, const T4& top) : m_left(left), m_bottom(bottom), m_right(right), m_top(top) {}

//    template<typename T1, typename T2, typename T3, typename T4>
//    TemplateBox(const T1& left, const T2& bottom, const T3& right, const T4& top) :
//        m_left(type_traits<C>::rounded(left)),
//        m_bottom(type_traits<C>::rounded(bottom)),
//        m_right(type_traits<C>::rounded(right)),
//        m_top(type_traits<C>::rounded(top)) {}

    template<typename T>
    TemplateBox(const TemplateBox<T>&b) : m_left(type_traits<C>::rounded(b.left())),
                                          m_bottom(type_traits<C>::rounded(b.bottom())),
                                          m_right(type_traits<C>::rounded(b.right())),
                                          m_top(type_traits<C>::rounded(b.top())) {}

    bool is_empty() const { return m_left > m_right || m_bottom > m_top; }
    C left() const { return m_left; }
    C bottom() const { return m_bottom; }
    C right() const { return m_right; }
    C top() const { return m_top; }
    C width() const { return is_empty() ? 0 : (m_right - m_left); }
    C height() const { return is_empty() ? 0 : (m_top - m_bottom); }
    C area() const { return is_empty() ? 0 : (m_right - m_left) * (m_top - m_bottom);}
    template<typename T>
    void set_left(T left) { m_left = left; }
    template<typename T>
    void set_bottom(T bottom) { m_bottom = bottom; }
    template<typename T>
    void set_right(T right) { m_right = right; }
    template<typename T>
    void set_top(T top) { m_top = top; }
    void clear() { m_left = 1; m_bottom = 1; m_right = -1; m_top = -1; }

    void update(const TemplatePoint<C>& p)
    {
        if (is_empty())
        {
            set_left(p.x());
            set_right(p.x());
            set_bottom(p.y());
            set_top(p.y());
        }
        else
        {
            if (p.x() < left()) set_left(p.x());
            if (p.x() > right()) set_right(p.x());
            if (p.y() < bottom()) set_bottom(p.y());
            if (p.y() > top()) set_top(p.y());
        }
    }

    void update(const TemplateBox<C>&b)
    {
        if (b.is_empty())
        {
            return;
        }

        if (is_empty())
        {
            *this = b;
        }
        else
        {
            if (b.left() < left()) set_left(b.left());
            if (b.right() > right()) set_right(b.right());
            if (b.bottom() < bottom()) set_bottom(b.bottom());
            if (b.top() > top()) set_top(b.top());
        }
    }

    void intersect(const TemplateBox<C> &b)
    {
        if (is_empty())
        {
            return;
        }

        if (b.is_empty())
        {
            *this = b;
            return;
        }

        if (b.left() > left())
        {
            set_left(b.left());
        }
        if (b.bottom() > bottom())
        {
            set_bottom(b.bottom());
        }
        if (b.right() < right())
        {
            set_right(b.right());
        }
        if (b.top() < top())
        {
            set_top(b.top());
        }
    }

    TemplateBox<C> intersected(const TemplateBox<C>&b)
    {
        TemplateBox<C> box(*this);
        box.intersect(b);
        return box;
    }

    void shift(const TemplatePoint<C> &p)
    {
        if (!is_empty())
        {
            set_left(left() + p.x());
            set_right(right() + p.x());
            set_bottom(bottom() + p.y());
            set_top(top() + p.y());
        }
    }

    TemplateBox<C> shifted(const TemplatePoint<C> &p) const
    {
        TemplateBox<C> b = *this;
        b.shift(p);
        return b;
    }

    void trans(const OasisTrans& t)
    {
        TemplatePoint<C> lb = t.trans(TemplatePoint<C>(left(), bottom()));
        TemplatePoint<C> rt = t.trans(TemplatePoint<C>(right(), top()));
        set_left(std::min(lb.x(), rt.x()));
        set_bottom(std::min(lb.y(), rt.y()));
        set_right(std::max(lb.x(), rt.x()));
        set_top(std::max(lb.y(), rt.y()));
    }

    TemplateBox<C> transed(const OasisTrans &t) const
    {
        TemplateBox<C> b = *this;
        b.trans(t);
        return b;
    }

    TemplateBox<float64> transedF(const OasisTrans &t) const
    {
        TemplateBox<float64> b = *this;
        b.trans(t);
        return b;
    }

    bool overlap(const TemplateBox<C> &other) const
    {
        if (is_empty() || other.is_empty())
        {
            return false;
        }
        return left() < other.right() &&
               right() > other.left() &&
               bottom() < other.top() &&
               top() > other.bottom();
    }


    static TemplateBox<C> world()
    {
        return TemplateBox<C>(std::numeric_limits<int64>::min(), std::numeric_limits<int64>::min(), std::numeric_limits<int64>::max(), std::numeric_limits<int64>::max());
    }
private:
    C m_left;
    C m_bottom;
    C m_right;
    C m_top;
};

typedef TemplateBox<int64> Box;
typedef TemplateBox<float64> BoxF;

template<typename C>
class TemplatePolygon
{
public:
    typedef typename std::vector<TemplatePoint<C> >::size_type size_type;

    void trans(const OasisTrans &tr)
    {
        for (uint32 i = 0; i < m_points.size(); ++i)
        {
            m_points[i] = tr.trans(m_points[i]);
        }
    }

    void shift(const TemplatePoint<C> &disp)
    {
        for (uint32 i = 0; i < m_points.size(); ++i)
        {
            m_points[i] = m_points[i] + disp;
        }
    }

    TemplatePolygon<C> transed(const OasisTrans &tr) const
    {
        TemplatePolygon<C> p(*this);
        p.trans(tr);
        return p;
    }

    TemplatePolygon<C> shifted(const TemplatePoint<C> &disp) const
    {
        TemplatePolygon<C> p(*this);
        p.shift(disp);
        return p;
    }

    TemplatePolygon<float64> transedF(const OasisTrans &tr) const
    {
        TemplatePolygon<float64> p(*this);
        p.trans(tr);
        return p;
    }

    TemplatePolygon<float64> shiftedF(const TemplatePoint<float64> &disp) const
    {
        TemplatePolygon<float64> p(*this);
        p.shift(disp);
        return p;
    }

    size_type size() const { return m_points.size(); }

    template<typename Iter>
    void assign(Iter b, Iter e)
    {
        m_points.assign(b, e);
        m_box.clear();
    }

    const std::vector<TemplatePoint<C> >& get_points() const { return m_points; }

    TemplateBox<C> bbox()
    {
        if (m_box.is_empty()) for(uint32 i = 0; i < m_points.size(); m_box.update(m_points[i]), ++i);
        return m_box;
    }

    bool is_manhattan() const
    {
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            size_t next = i + 1;
            if (next == m_points.size())
            {
                next = 0;
            }
            const TemplatePoint<C>& p = m_points[i];
            const TemplatePoint<C>& np = m_points[next];
            if (p.x() != np.x() && p.y() != np.y())
            {
                return false;
            }
        }
        return true;
    }

    bool is_box() const
    {
        return m_points.size() == 4 && this->is_manhattan();
    }

private:
    std::vector<TemplatePoint<C> > m_points;
    TemplateBox<C> m_box;
};

typedef TemplatePolygon<int64> Polygon;
typedef TemplatePolygon<float64> PolygonF;

const uint8 OasisMagic[] = {'%', 'S', 'E', 'M', 'I', '-', 'O', 'A', 'S', 'I', 'S', '\r', '\n'};

enum OasisRecordId
{
    ORID_Pad = 0,
    ORID_Start = 1,
    ORID_End = 2,
    ORID_Cellname = 3,
    ORID_CellnameRef = 4,
    ORID_Textstring = 5,
    ORID_TextstringRef = 6,
    ORID_Propname = 7,
    ORID_PropnameRef = 8,
    ORID_Propstring = 9,
    ORID_PropstringRef = 10,
    ORID_Layername = 11,
    ORID_LayernameText = 12,
    ORID_CellRef = 13,
    ORID_Cell = 14,
    ORID_XYAbsolute = 15,
    ORID_XYRelative = 16,
    ORID_Placement = 17,
    ORID_PlacementTrans = 18,
    ORID_Text = 19,
    ORID_Rectangle = 20,
    ORID_Polygon = 21,
    ORID_Path = 22,
    ORID_Trapezoid = 23,
    ORID_TrapezoidA = 24,
    ORID_TrapezoidB = 25,
    ORID_CTrapezoid = 26,
    ORID_Circle = 27,
    ORID_Property = 28,
    ORID_PropertyLast = 29,
    ORID_XName = 30,
    ORID_XNameRef = 31,
    ORID_XElement = 32,
    ORID_XGeometry = 33,
    ORID_Cblock = 34,

    ORID_Max
};

class OasisReal
{
    friend class OasisReader;
    friend class OasisWriter;
public:
    enum Type
    {
        ORT_PositiveInteger = 0,
        ORT_NegativeInteger = 1,
        ORT_PositiveReciprocal = 2,
        ORT_NegativeReciprocal = 3,
        ORT_PositiveRatio = 4,
        ORT_NegativeRatio = 5,
        ORT_Float32 = 6,
        ORT_Float64 = 7,

        ORT_Max
    };
    OasisReal() { m_type = ORT_Float64; m_value = 0.0; }
    explicit OasisReal(float64 v) { m_type = ORT_Float64; m_value = v; }
    const float64& value() const { return m_value; }
    bool is_float() const { return m_type == ORT_Float32 || m_type == ORT_Float64; }

    bool operator==(const OasisReal& r) const
    {
        return m_type == r.m_type && m_value == r.m_value;
    }

    bool operator!=(const OasisReal& r) const
    {
        return !operator==(r);
    }
private:
    Type m_type;
    uint64 m_numerator;
    uint64 m_denominator;
    float64 m_value;
};

class OasisString
{
    friend class OasisReader;
    friend class OasisWriter;
public:
    OasisString() {}
    OasisString(const std::string& value) : m_value(value) {}
    const std::string& value() const { return m_value; }
    void set_value(const std::string& value) { m_value = value; }
    bool is_astring() const;
    bool is_bstring() const { return true; }
    bool is_nstring() const;

    bool operator==(const OasisString& s) const
    {
        return m_value == s.m_value;
    }

    bool operator!=(const OasisString& s) const
    {
        return !operator==(s);
    }
private:
    enum
    {
        AsciiSpace = 0x20,
        AsciiPrintableMin = 0x21,
        AsciiPrintableMax = 0x7E
    };
    std::string m_value;
};

class OasisDelta
{
    friend class OasisReader;
    friend class OasisWriter;
    friend class OasisPointListBuilder;
    friend class OasisRepetition;
public:
    enum Direction
    {
        East = 0,
        North = 1,
        West = 2,
        South = 3,
        NorthEast = 4,
        NorthWest = 5,
        SouthWest = 6,
        SouthEast = 7,
        Any = 8,
        Horizontal = 9,
        Vertical = 10
    };
    enum Type
    {
        ODT_Delta1,
        ODT_Delta2,
        ODT_Delta3,
        ODT_DeltaG,

        ODT_Max
    };

    OasisDelta() : m_type(ODT_Max), m_dir(Any), m_dx(0), m_dy(0) {}
    OasisDelta(int64 dx, int64 dy) : m_type(ODT_DeltaG), m_dir(Any), m_dx(dx), m_dy(dy) {}
    OasisDelta(const Point& p) : m_type(ODT_DeltaG), m_dir(Any), m_dx(p.x()), m_dy(p.y()) {}

    int64 dx() const { return m_dx; }
    int64 dy() const { return m_dy; }

    void setValue(const Direction& dir, const uint64& value);
    void getValue(Direction& dir, uint64& value) const;

    bool operator==(const OasisDelta& d) const
    {
        return m_type == d.m_type && m_dir == d.m_dir && m_dx == d.m_dx && m_dy == d.m_dy;
    }

    bool operator!=(const OasisDelta& d) const
    {
        return !operator==(d);
    }

    bool operator<(const OasisDelta& d) const
    {
        if(m_type != d.m_type)
        {
            return m_type < d.m_type;
        }
        if(m_dir != d.m_dir)
        {
            return m_dir < d.m_dir;
        }
        if(m_dx != d.m_dx)
        {
            return m_dx < d.m_dx;
        }
        return m_dy < d.m_dy;
    }
private:
    Type m_type;
    Direction m_dir;
    int64 m_dx;
    int64 m_dy;
};

class OasisRepetition
{
    friend class OasisReader;
    friend class OasisWriter;
    friend class OasisRepetitionBuilder;
public:
    enum Type
    {
        ORT_ReusePrevious = 0,
        ORT_Regular2D = 1,
        ORT_HorizontalRegular = 2,
        ORT_VerticalRegular = 3,
        ORT_HorizontalIrregular = 4,
        ORT_HorizontalIrregularGrid = 5,
        ORT_VerticalIrregular = 6,
        ORT_VerticalIrregularGrid = 7,
        ORT_DiagonalRegular2D = 8,
        ORT_DiagonalRegular1D = 9,
        ORT_Arbitory = 10,
        ORT_ArbitoryGrid = 11,

        ORT_Max
    };
    enum Dim
    {
        DimX = 0,
        DimY = 1,
        DimN = 0,
        DimM = 1,

        Dim1D = 1,
        Dim2D = 2
    };

    OasisRepetition() : m_type(ORT_Max) {}

    void get_disps(std::vector<Point>& disps) const;
    void get_bbox(Box& bbox) const;
    uint64 get_dimension() const;

    bool operator==(const OasisRepetition& r) const
    {
        if(m_type != r.m_type)
        {
            return false;
        }

        switch(m_type)
        {
            case ORT_Regular2D:
                return m_dimensionX == r.m_dimensionX && m_dimensionY == r.m_dimensionY && m_spaces == r.m_spaces;
            case ORT_HorizontalRegular:
                return m_dimensionX == r.m_dimensionX && m_spaces == r.m_spaces;
            case ORT_VerticalRegular:
                return m_dimensionY == r.m_dimensionY && m_spaces == r.m_spaces;
            case ORT_HorizontalIrregular:
                return m_dimensionX == r.m_dimensionX && m_spaces == r.m_spaces;
            case ORT_HorizontalIrregularGrid:
                return m_dimensionX == r.m_dimensionX && m_grid == r.m_grid && m_spaces == r.m_spaces;
            case ORT_VerticalIrregular:
                return m_dimensionY == r.m_dimensionY && m_spaces == r.m_spaces;
            case ORT_VerticalIrregularGrid:
                return m_dimensionY == r.m_dimensionY && m_grid == r.m_grid && m_spaces == r.m_spaces;
            case ORT_DiagonalRegular2D:
                return m_dimensionM == r.m_dimensionM && m_dimensionN == r.m_dimensionN && m_deltas == r.m_deltas;
            case ORT_DiagonalRegular1D:
                return m_dimension == r.m_dimension && m_deltas == r.m_deltas;
            case ORT_Arbitory:
                return m_dimension == r.m_dimension && m_deltas == r.m_deltas;
            case ORT_ArbitoryGrid:
                return m_dimension == r.m_dimension && m_grid == r.m_grid && m_deltas == r.m_deltas;
            default:
                oas_assert(false);
        }
        return false;
    }

    bool operator!=(const OasisRepetition& r) const
    {
        return !operator==(r);
    }

    bool valid() const { return m_type != ORT_Max; }
private:
    void reset()
    {
        m_type = ORT_Max;
        m_deltas.clear();
        m_spaces.clear();
    }

private:
    Type m_type;
    union
    {
        uint64 m_dimension;
        uint64 m_dimensionX;
        uint64 m_dimensionN;
    };
    union
    {
        uint64 m_dimensionY;
        uint64 m_dimensionM;
    };
    uint64 m_grid;

    std::vector<OasisDelta> m_deltas;
    std::vector<int64> m_spaces;
};

class OasisPointList
{
    friend class OasisReader;
    friend class OasisWriter;
    friend class OasisPointListBuilder;
public:
    enum Type
    {
        OPLT_ManhattanHorizontalFirst = 0,
        OPLT_ManhattanVerticalFirst = 1,
        OPLT_Manhattan = 2,
        OPLT_Octangular = 3,
        OPLT_AnyAngle = 4,
        OPLT_AnyAngleAdjust = 5,

        OPLT_Max
    };
    OasisPointList() : m_type(OPLT_Max) {}
    unsigned int size() const { return m_deltas.size(); }

    bool operator==(const OasisPointList& l) const
    {
        return m_type == l.m_type && m_deltas == l.m_deltas;
    }

    bool operator!=(const OasisPointList& l) const
    {
        return !operator==(l);
    }

    bool operator<(const OasisPointList& l) const
    {
        if(m_type != l.m_type)
        {
            return m_type < l.m_type;
        }
        if(m_deltas.size() != l.m_deltas.size())
        {
            return m_deltas.size() < l.m_deltas.size();
        }
        return m_deltas < l.m_deltas;
    }
private:
    Type m_type;
    std::vector<OasisDelta> m_deltas;
};

class OasisPropValue
{
    friend class OasisReader;
    friend class OasisWriter;
    friend class OasisPropertyBuilder;
public:
    OasisPropValue() : m_type(OPVT_Max), m_uint_value(0), m_propstring_index(0) {}
    enum Type
    {
        OPVT_PositiveInteger = 0,
        OPVT_NegativeInteger = 1,
        OPVT_PositiveReciprocal = 2,
        OPVT_NegativeReciprocal = 3,
        OPVT_PositiveRatio = 4,
        OPVT_NegativeRatio = 5,
        OPVT_Float32 = 6,
        OPVT_Float64 = 7,
        OPVT_UnsignedInteger = 8,
        OPVT_SignedInteger = 9,
        OPVT_AString = 10,
        OPVT_BString = 11,
        OPVT_NString = 12,
        OPVT_RefAString = 13,
        OPVT_RefBString = 14,
        OPVT_RefNString = 15,

        OPVT_Max
    };

    bool is_string() const { return m_type >= OPVT_AString && m_type <= OPVT_NString; }
    bool is_refstring() const { return m_type >= OPVT_RefAString && m_type <= OPVT_RefNString; }
    uint64 get_refnum() const { return m_ref_value; }
    const OasisString& get_string() const { return m_string_value; }
    void set_index(int64 index) { m_propstring_index = index; }

    bool operator==(const OasisPropValue& v) const
    {
        if(m_type != v.m_type)
        {
            return false;
        }

        if(static_cast<OasisReal::Type>(m_type) < OasisReal::ORT_Max)
        {
            return m_real_value == v.m_real_value;
        }

        switch(m_type)
        {
            case OPVT_UnsignedInteger:
                return m_uint_value == v.m_uint_value;
            case OPVT_SignedInteger:
                return m_int_value == v.m_int_value;
            case OPVT_AString:
            case OPVT_BString:
            case OPVT_NString:
                return m_string_value == v.m_string_value;
            case OPVT_RefAString:
            case OPVT_RefBString:
            case OPVT_RefNString:
                return m_ref_value == v.m_ref_value;
            default:
                oas_assert(false);
        }
        return false;
    }

    bool operator!=(const OasisPropValue& v) const
    {
        return !operator==(v);
    }
private:
    Type m_type;
    union
    {
        uint64 m_uint_value;
        int64 m_int_value;
        uint64 m_ref_value;
    };
    OasisReal m_real_value;
    OasisString m_string_value;
    int64 m_propstring_index;
};

class OasisInterval
{
    friend class OasisReader;
    friend class OasisWriter;
    friend class OasisIntervalBuilder;
    friend class OasisLayerName;
public:
    enum Type
    {
        OIT_All = 0,
        OIT_BoundBellow = 1,
        OIT_BoundAbove = 2,
        OIT_Exact = 3,
        OIT_BoundAll = 4,

        OIT_Max
    };
    OasisInterval() : m_type(OIT_Max), m_lower(0), m_upper(0) {}

    bool contains(uint32 value) const
    {
        return value >= m_lower && value <= m_upper;
    }
    bool operator==(const OasisInterval& i) const
    {
        return m_type == i.m_type && m_lower == i.m_lower && m_upper == i.m_upper;
    }

    bool operator!=(const OasisInterval& i) const
    {
        return !operator==(i);
    }

    static const uint32 Infinite = 0xFFFFFFFF;
private:
    Type m_type;
    uint32 m_lower;
    uint32 m_upper;
};


//info byte enum
namespace PlacementInfoByte
{
    enum
    {
        HasExplicitCell = 0x80,
        HasRefnum       = 0x40,
        HasX            = 0x20,
        HasY            = 0x10,
        HasRepetition   = 0x08,
        HasMag          = 0x04,
        HasAngle        = 0x02,
        HasMirror       = 0x01,

        AngleMask       = 0x06,
        Angle0          = 0x0,
        Angle90         = 0x02,
        Angle180        = 0x04,
        Angle270        = 0x06
    };
}

namespace TextInfoByte
{
    enum
    {
        HasExplicitText = 0x40,
        HasRefnum       = 0x20,
        HasX            = 0x10,
        HasY            = 0x08,
        HasRepetition   = 0x04,
        HasTexttype     = 0x02,
        HasTextlayer    = 0x01
    };
}

namespace RectangleInfoByte
{
    enum
    {
        IsSquare        = 0x80,
        HasWidth        = 0x40,
        HasHeight       = 0x20,
        HasX            = 0x10,
        HasY            = 0x08,
        HasRepetition   = 0x04,
        HasDatatype     = 0x02,
        HasLayer        = 0x01
    };
}

namespace PolygonInfoByte
{
    enum
    {
        HasPointList    = 0x20,
        HasX            = 0x10,
        HasY            = 0x08,
        HasRepetition   = 0x04,
        HasDatatype     = 0x02,
        HasLayer        = 0x01
    };
}

namespace PathInfoByte
{
    enum
    {
        HasExtension    = 0x80,
        HasHalfWidth    = 0x40,
        HasPointList    = 0x20,
        HasX            = 0x10,
        HasY            = 0x08,
        HasRepetition   = 0x04,
        HasDatatype     = 0x02,
        HasLayer        = 0x01
    };

    enum
    {
        ReuseLastExtension  = 0,
        FlushExtension      = 1,
        HalfwidthExtension  = 2,
        ExplicitExtension   = 3
    };
}

namespace TrapezoidInfoByte
{
    enum
    {
        IsVertical      = 0x80,
        HasWidth        = 0x40,
        HasHeight       = 0x20,
        HasX            = 0x10,
        HasY            = 0x08,
        HasRepetition   = 0x04,
        HasDatatype     = 0x02,
        HasLayer        = 0x01
    };
}

namespace CTrapezoidInfoByte
{
    enum
    {
        HasTrapType     = 0x80,
        HasWidth        = 0x40,
        HasHeight       = 0x20,
        HasX            = 0x10,
        HasY            = 0x08,
        HasRepetition   = 0x04,
        HasDatatype     = 0x02,
        HasLayer        = 0x01
    };
}

namespace CircleInfoByte
{
    enum
    {
        HasRadius       = 0x20,
        HasX            = 0x10,
        HasY            = 0x08,
        HasRepetition   = 0x04,
        HasDatatype     = 0x02,
        HasLayer        = 0x01
    };
}

namespace PropertyInfoByte
{
    enum
    {
        ValueCountMask  = 0xf0,
        ReuseLastValue  = 0x08,
        HasExplicitName = 0x04,
        HasRefnum       = 0x02,
        IsStandard      = 0x01
    };
}

namespace XGeometryInfoByte
{
    enum
    {
        HasX            = 0x10,
        HasY            = 0x08,
        HasRepetition   = 0x04,
        HasDatatype     = 0x02,
        HasLayer        = 0x01
    };
}

enum GDSRecordId
{
    GRID_Header         = 0x0002,
    GRID_BgnLib         = 0x0102,
    GRID_LibName        = 0x0206,
    GRID_Units          = 0x0305,
    GRID_EndLib         = 0x0400,
    GRID_BgnStr         = 0x0502,
    GRID_StrName        = 0x0606,
    GRID_EndStr         = 0x0700,
    GRID_Boundary       = 0x0800,
    GRID_Path           = 0x0900,
    GRID_Sref           = 0x0a00,
    GRID_Aref           = 0x0b00,
    GRID_Text           = 0x0c00,
    GRID_Layer          = 0x0d02,
    GRID_Datatype       = 0x0e02,
    GRID_Width          = 0x0f03,
    GRID_XY             = 0x1003,
    GRID_EndEl          = 0x1100,
    GRID_Sname          = 0x1206,
    GRID_ColRow         = 0x1302,
    GRID_Node           = 0x1500,
    GRID_Texttype       = 0x1602,
    GRID_Presentation   = 0x1701,
    GRID_String         = 0x1906,
    GRID_Strans         = 0x1a01,
    GRID_Mag            = 0x1b05,
    GRID_Angle          = 0x1c05,
    GRID_RefLibs        = 0x1f06,
    GRID_Fonts          = 0x2006,
    GRID_Pathtype       = 0x2102,
    GRID_Generations    = 0x2202,
    GRID_AttrTable      = 0x2306,
    GRID_Eflags         = 0x2601,
    GRID_Nodetype       = 0x2a02,
    GRID_PropAttr       = 0x2b02,
    GRID_PropValue      = 0x2c06,
    GRID_Box            = 0x2d00,
    GRID_Boxtype        = 0x2e02,
    GRID_Plex           = 0x2f03,
    GRID_BgnExtn        = 0x3003,
    GRID_EndExtn        = 0x3103,
    GRID_StrClass       = 0x3401,
    GRID_Format         = 0x3602,
    GRID_Mask           = 0x3706,
    GRID_EndMasks       = 0x3800,
    GRID_LibDirSize     = 0x3902,
    GRID_SrfName        = 0x3a06,
    GRID_LibSecur       = 0x3b02,

    GRID_Max            = 0x3c
};

namespace StransInfoByte
{
    enum
    {
        HasMirror       = 0x8000,
        HasAbsMag       = 0x0004,
        HasAbsAngle     = 0x0002
    };
}

namespace GDSPathInfoByte
{
    enum
    {
        FlushExtension      = 0x00,
        RoundExtension      = 0x01,
        SquareExtension     = 0x02,
        ExplicitExtension   = 0x04
    };
}

class LDType
{
public:
    LDType(int64 l = -1, int64 t = -1) : layer(l), datatype(t) {}

    bool operator<(const LDType& rh) const { return layer < rh.layer || (layer == rh.layer && datatype < rh.datatype); }

    int64 layer;
    int64 datatype;
};

#define OAS_DBG     OasisDebug::loglevel() >= 10 && logger::info
#define OAS_WARN    logger::warning
#define OAS_ERR     logger::error
#define OAS_INFO    OasisDebug::loglevel() >= 1 && logger::info
#define OAS_ENDL    ""
class OasisDebug
{
public:
    static void loglevel(int32 level) { m_level = level; }
    static int32 loglevel() { return m_level; }
    static void oasis_error_disp(bool disp) { m_error_disp = disp;}
    static bool oasis_error_disp() { return m_error_disp;}
private:
    static int32 m_level;
    static bool m_error_disp;
};

class OasisComply
{
public:
    static void setcomply(bool comply) { m_comply = comply; }
    static bool comply() { return m_comply; }
private:
    static bool m_comply;
};

}

#endif
