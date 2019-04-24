#ifndef RENDER_CELL_CACHE_H
#define RENDER_CELL_CACHE_H

#include "oasis_types.h"
#include "render_bitmap.h"

#include <QMutex>

namespace render
{


struct CellKey
{
public:
    CellKey(const oasis::OasisTrans& trans, oasis::int64 n): m_trans(trans), m_cell_index(n), m_epsilon(1e-10) { }

    oasis::OasisTrans m_trans;
    oasis::int64 m_cell_index;
    oasis::float64 m_epsilon;

    bool operator< (const CellKey& cell_key) const
    {

        if( m_cell_index != cell_key.m_cell_index)
        {
            return m_cell_index < cell_key.m_cell_index;
        }

        if(!operator==(cell_key))
        {
            if( fabs(m_trans.dispF().x() - cell_key.m_trans.dispF().x()) > m_epsilon)
            {
                return m_trans.dispF().x() < cell_key.m_trans.dispF().x();
            }

            if( fabs(m_trans.dispF().y() - cell_key.m_trans.dispF().y()) > m_epsilon)
            {
                return m_trans.dispF().y() < cell_key.m_trans.dispF().y();
            }

            if(fabs(m_trans.angle() - cell_key.m_trans.angle()) > m_epsilon)
            {
                return m_trans.angle() < cell_key.m_trans.angle();
            }

            if(fabs(m_trans.mag() - cell_key.m_trans.mag()) > m_epsilon)
            {
                return m_trans.mag() < cell_key.m_trans.mag();
            }
            return false;
        }
        return false;
    }

    bool operator==(const CellKey& cell_key) const
    {
        if(m_cell_index != cell_key.m_cell_index)
        {
            return false;
        }
        if(fabs(m_trans.mag() - cell_key.m_trans.mag()) < m_epsilon &&
           fabs(m_trans.angle() - cell_key.m_trans.angle()) < m_epsilon &&
           fabs(m_trans.dispF().x() - cell_key.m_trans.dispF().x()) < m_epsilon &&
           fabs(m_trans.dispF().y() - cell_key.m_trans.dispF().y()) < m_epsilon)
        {
            return true;
        }

        return false;
    }
};


struct BitmapInfo
{
public:
    BitmapInfo():bitmap(0),layer_index(-1) {}
    BitmapInfo(render::Bitmap* b, int index): bitmap(b), layer_index(index) {}
    render::Bitmap* bitmap;
    int layer_index;
};

struct CellCacheStatus
{
public:
    CellCacheStatus():
        m_hits(0),
        m_vertex(0) {  }

    void clean_cache()
    {
        for(auto it = m_fill_groups.begin(); it != m_fill_groups.end();it++)
        {
            render::Bitmap* bitmap = (it->second).bitmap;
            delete bitmap;
        }

        for(auto it = m_contour_groups.begin(); it != m_contour_groups.end();it++)
        {
            render::Bitmap* bitmap = (it->second).bitmap;
            delete bitmap;
        }

        if(m_vertex)
        {
            delete m_vertex;
        }
        m_vertex = 0;
    }

    oasis::int64 m_hits;
    std::map<oasis::LDType, BitmapInfo> m_fill_groups;
    std::map<oasis::LDType, BitmapInfo> m_contour_groups;
    render::Bitmap* m_vertex;
    oasis::PointF m_disp;
};

class CellCacheManager
{
public:
    typedef std::map<CellKey, CellCacheStatus> cell_cache_type;
    typedef typename cell_cache_type::iterator cell_cache_iter;
    CellCacheManager() { }

    ~CellCacheManager()
    {
        cleanup();
    }

    cell_cache_iter begin();

    cell_cache_iter end();

    void set(const CellKey& key, const CellCacheStatus& status);

    CellCacheStatus& get(const CellKey& key);

    bool find_key(const CellKey& key);

    void cleanup();
private:
    cell_cache_type m_cache_map;
    QMutex m_mutex_lock;
};
}

#endif // RENDER_CELL_CACHE_H
