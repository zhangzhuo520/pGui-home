#ifndef RENDER_SNAP_H
#define RENDER_SNAP_H
#include <utility>
#include "oasis_types.h"

namespace render
{

class RenderFrame;

std::pair<bool,Oasis::OasisPointF> snap_point(RenderFrame* frame, Oasis::OasisPointF p1, Oasis::float64 snap_range);

std::pair<bool,Oasis::OasisEdgeF> snap_edge(RenderFrame* frame, Oasis::OasisPointF p1, Oasis::float64 snap_range);
}


#endif // RENDER_SNAP_H
