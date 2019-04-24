#ifndef COUNT_MARK_VISITOR_H
#define COUNT_MARK_VISITOR_H
#include "cell_visitor.h"
#include <map>
#include <string>

namespace oasis
{

class CountMarkVisitor : public CellVisitor
{
public:
    CountMarkVisitor(const std::string& subname, std::map<std::string, std::string>& parent_to_subname, std::map<std::string, int>& corner_to_count, std::map<std::string, std::vector<oasis::OasisTrans>> &instances_map) :
        m_count(0),
        m_subname(subname),
        m_parent_to_subname(parent_to_subname),
        m_corner_to_count(corner_to_count),
        m_instances(instances_map) {}
    virtual void visit_geometries(const std::vector<OasisElement*>& elements) {}
    virtual void visit_texts(const std::vector<OasisElement*>& elements) {}
    virtual void visit_instances(const std::vector<OasisElement*>& elements);

    void set_parent_name(const std::string& parent) { m_parent_name = parent; }
    std::string get_parent_name() const { return m_parent_name; }
    int32 count() { return m_count; }
private:
    int32 m_count;
    std::string m_subname;
    std::map<std::string, std::string>& m_parent_to_subname;
    std::map<std::string, int>& m_corner_to_count;
    std::map<std::string, std::vector<OasisTrans>>& m_instances;
    std::string m_parent_name;
};

}
#endif
