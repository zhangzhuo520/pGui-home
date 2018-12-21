#ifndef RENDER_BITMAPSTOIMAGE_H
#define RENDER_BITMAPSTOIMAGE_H

#include <vector>
#include "render_view_op.h"

class QMutex;
class QImage;

namespace render {

class Bitmap;
class Pattern;
class LineStyle;

void
bitmaps_to_image(
    const std::vector<render::ViewOp> &view_ops_in,
    const std::vector<render::Bitmap*> &bitmaps,
    const render::Pattern &dp,
    const render::LineStyle &ls,
    QImage* p,
    unsigned int width,
    unsigned int height,
    QMutex* mutex);

void
bitmap_to_bitmap(
    const render::Bitmap &bitmap,
    unsigned char * data,
    unsigned int width,
    unsigned int height);
}
#endif

