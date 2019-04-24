#ifndef RENDER_SNAP_H
#define RENDER_SNAP_H
#include <utility>
#include "oasis_types.h"

namespace render
{

class RenderFrame;

std::pair<bool,oasis::PointF> snap_point(RenderFrame* frame, oasis::PointF p1, oasis::float64 snap_range, int mode);

std::pair<bool,oasis::EdgeF> snap_edge(RenderFrame* frame, oasis::PointF p1, oasis::float64 snap_range, int mode);
}


#endif // RENDER_SNAP_H
