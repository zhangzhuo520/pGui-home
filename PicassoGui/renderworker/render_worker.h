#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include "OasisLayout.h"

namespace render{

class Bitmap;

class RenderWorker
{
public:
    RenderWorker(
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
        Bitmap* vertex);

    void run();

private:
    Oasis::OasisLayout * m_layout;
    Oasis::int64 m_infoindex;
    Oasis::OasisBox m_q_box;
    Oasis::OasisTrans m_trans;
    Oasis::LDType m_ld;
    Oasis::uint32 m_width;
    Oasis::uint32 m_height;
    Oasis::float64 m_resolution;
    Bitmap* m_contour;
    Bitmap* m_fill;
    Bitmap* m_vertex;
};

}
#endif // RENDER_WORKER_H
