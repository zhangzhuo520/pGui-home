#ifndef DRAWBITMAPVISITOR_H
#define DRAWBITMAPVISITOR_H
#include "cell_visitor.h"

namespace render
{
    class Bitmap;
}
namespace oasis
{

class DrawBitmapVisitor : public CellVisitor
{
public:
    DrawBitmapVisitor(
        const LDType &ld,
        uint32 width,
        uint32 height,
        float64 resolution,
        render::Bitmap* contour,
        render::Bitmap* fill,
        render::Bitmap* vertex) :
        m_ld(ld),
        m_width(width),
        m_height(height),
        m_resolution(resolution),
        m_contour(contour),
        m_fill(fill),
        m_vertex(vertex) { }

    virtual void visit_instances(const std::vector<OasisElement*>& elements);
    virtual void visit_geometries(const std::vector<OasisElement*>& elements);
    virtual void visit_texts(const std::vector<OasisElement*>& elements) { }

private:
    LDType m_ld;
    uint32 m_width;
    uint32 m_height;
    float64 m_resolution;
    render::Bitmap* m_contour;
    render::Bitmap* m_fill;
    render::Bitmap* m_vertex;
};

}

#endif
