#ifndef RENDER_SNAP_H
#define RENDER_SNAP_H
#include <utility>
#include "oasis_types.h"

namespace render
{

class RenderFrame;

std::pair<bool,Oasis::OasisPoint> snap_point(RenderFrame* frame, Oasis::OasisPoint p1, int snap_range);

std::pair<bool,Oasis::OasisEdge> snap_edge(RenderFrame* frame, Oasis::OasisPoint p1, int snap_range);
}


#endif // RENDER_SNAP_H
