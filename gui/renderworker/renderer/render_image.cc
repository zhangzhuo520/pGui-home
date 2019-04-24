#include "render_image.h"
#include "render_viewport.h"

#include <QImage>
#include <QPainter>
#include <QString>
#include <QDebug>

namespace render
{
RTSImage::RTSImage(int width,
                   int height,
                   int left,
                   int bottom,
                   int pixel_space,
                   std::string& file_path,
                   double dbu,
                   RenderObjectWidget *frame):
                   BackgroundObject(frame),
                   m_width(width), m_height(height),
                   m_left(left), m_bottom(bottom),
                   m_pixel_space(pixel_space),
                   m_file_path(file_path),  m_image(0),
                   m_dbu(dbu)

{

}

RTSImage::~RTSImage()
{
    delete m_image;
}

void RTSImage::render_background(const Viewport& vp, RenderObjectWidget* widget)
{
    if(!m_image)
    {
        m_image = new QImage(m_width, m_height, QImage::Format_Indexed8);
        qDebug() << "create image";
        if(!m_image->load(QString::fromStdString(m_file_path)))
        {
            qDebug() << "load error";
            std::string result = "file path:" + m_file_path + "has unexpected errors";
            throw LoadImageError(result);
            return ;
        }

    }

    const oasis::OasisTrans& trans = vp.trans();

    oasis::PointF origin_lb(m_left * 0.001, m_bottom * 0.001);
    oasis::PointF origin_rt((m_left + m_width * m_pixel_space) * 0.001, (m_bottom + m_height * m_pixel_space) * 0.001);
    oasis::PointF current_lb = trans.transF(origin_lb); // bitmap_space
    oasis::PointF current_rt = trans.transF(origin_rt);
    oasis::PointF pixel_placement(current_lb.x(), vp.height() - 1 - current_rt.y());

//    qDebug() << "origin lb x:" << origin_lb.x() << " origin y" << origin_lb.y();
//    qDebug() << "origin rt x:" << origin_rt.x() << " origin y" << origin_rt.y();
//    qDebug() << "current x:" << current_lb.x() << "current y" << current_lb.y();
//    qDebug() << "m_left" << m_left << "m_bottom" << m_bottom << "dbu" << m_dbu;
    int current_width = rint(current_rt.x() - current_lb.x());
    int current_height = rint(current_rt.y() - current_lb.y());
    qDebug() << current_width << current_height;
    QImage new_image = m_image->scaled(current_width, current_height);

    QImage& bg_image = widget->background_image();
    QPainter painter(&bg_image);
//    painter.setCompositionMode(QPainter::CompositionMode_Source);
//    painter.fillRect(bg_image.rect(), Qt::transparent);
//    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(pixel_placement.x(), pixel_placement.y(), new_image);
//    painter.setComposition(QPainter::CompositionMode_Destination);
//    painter.drawImage(bg_image.rect(), QColor(0,0, 0, aplha));
    painter.end();
}

}
