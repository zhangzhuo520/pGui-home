#include "render_cell_cache.h"
#include <QDebug>
#include <QMutexLocker>

namespace render
{
CellCacheManager::cell_cache_iter CellCacheManager::begin()
{
    return m_cache_map.begin();
}

CellCacheManager::cell_cache_iter CellCacheManager::end()
{
    QMutexLocker locker(&m_mutex_lock);
    return m_cache_map.end();
}

CellCacheStatus&  CellCacheManager::get(const CellKey& key)
{
    QMutexLocker locker(&m_mutex_lock);
    return m_cache_map[key];
}

bool CellCacheManager::find_key(const CellKey &key)
{
    QMutexLocker locker(&m_mutex_lock);
    cell_cache_iter it = m_cache_map.find(key);
    if(it == m_cache_map.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void CellCacheManager::set(const CellKey& key, const CellCacheStatus& status)
{
    QMutexLocker locker(&m_mutex_lock);
    m_cache_map.insert(std::make_pair(key, status));
}

void CellCacheManager::cleanup()
{
    QMutexLocker locker(&m_mutex_lock);
    for(auto it =  m_cache_map.begin(); it != m_cache_map.end(); it++)
    {
        it->second.clean_cache();
    }

    m_cache_map.clear();
    std::map<CellKey, CellCacheStatus>().swap(m_cache_map);
}
}
