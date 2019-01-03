#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include "oasis_layout.h"
#include <QRunnable>

namespace render{

class Bitmap;

class RenderWorker: public QRunnable
{
public:
    RenderWorker(
        oasis::OasisLayout* layout,
        oasis::int64 infoindex,
        oasis::Box& q_box,
        oasis::OasisTrans& trans,
        const oasis::LDType& ld,
        oasis::uint32 width,
        oasis::uint32 height,
        oasis::float64 resolution,
        Bitmap* contour,
        Bitmap* fill,
        Bitmap* vertex);

    ~RenderWorker();
    void run();

private:
    oasis::OasisLayout * m_layout;
    oasis::int64 m_infoindex;
    oasis::Box m_q_box;
    oasis::OasisTrans m_trans;
    oasis::LDType m_ld;
    oasis::uint32 m_width;
    oasis::uint32 m_height;
    oasis::float64 m_resolution;
    Bitmap* m_contour;
    Bitmap* m_fill;
    Bitmap* m_vertex;
};

}
#endif // RENDER_WORKER_H
