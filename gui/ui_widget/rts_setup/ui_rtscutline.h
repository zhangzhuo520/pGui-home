#ifndef UI_RTSCUTLINE_H
#define UI_RTSCUTLINE_H
#include <QImage>
class QImage;
class RtsCutline
{
public:
    RtsCutline();
    void get_image();


private:
    QImage m_image;
};

#endif // UI_RTSCUTLINE_H
