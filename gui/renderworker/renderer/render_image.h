#ifndef RENDER_IMAGE_H
#define RENDER_IMAGE_H
#include "render_view_object.h"
#include <QColor>

class QImage;

namespace render
{

class Viewport;
class LoadImageError: public std::logic_error
{
public:
    explicit LoadImageError(const std::string& s): std::logic_error(s) { }
};

class RTSImage: public BackgroundObject
{
public:
    RTSImage(int width, int height, int left, int bottom, int pixel_space, std::string& file_path, double dbu, RenderObjectWidget* frame = 0);
    ~RTSImage();
    virtual void render_background(const Viewport& vp, RenderObjectWidget* widget = 0);

    std::string file_path() const
    {
        return m_file_path;
    }

private:

    QColor m_color;
    int m_width;
    int m_height;
    long m_left;
    long m_bottom;
    int m_pixel_space;
    std::string m_file_path;
    QImage* m_image;
    double m_dbu;
};

}
#endif // RENDER_IMAGE_H
