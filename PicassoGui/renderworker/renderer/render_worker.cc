#ifndef RENDER_WORKER_CC
#define RENDER_WORKER_CC

#include "render_worker.h"

namespace render
{
RenderWorker::RenderWorker(
    Oasis::OasisLayout* layout,
    Oasis::int64 infoindex,
    Oasis::OasisBox& q_box,
    Oasis::OasisTrans& trans,
    const Oasis::LDType& ld,
    Oasis::uint32 width,
    Oasis::uint32 height,
    Oasis::float64 resolution,
    Bitmap* contour,
    Bitmap* fill,
    Bitmap* vertex)
    :m_layout(layout),
     m_infoindex(infoindex),
     m_q_box(q_box),
     m_trans(trans),
     m_ld(ld),
     m_width(width),
     m_height(height),
     m_resolution(resolution),
     m_contour(contour),
     m_fill(fill),
     m_vertex(vertex)
{

}

RenderWorker::~RenderWorker()
{

}


void RenderWorker::run()
{
    m_layout->DrawBitmap(m_infoindex, m_q_box, m_trans, m_ld, m_width, m_height, m_resolution, m_contour, m_fill, m_vertex);
}

}

#endif // RENDER_WORKER_CC
