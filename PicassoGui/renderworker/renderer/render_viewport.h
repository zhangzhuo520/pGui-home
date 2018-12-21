#ifndef RENDER_VIEWPORT_H
#define RENDER_VIEWPORT_H
#include "oasis_types.h"

namespace render{

class Viewport
{
public:
    Viewport();

    Viewport(unsigned int width, unsigned int height, const Oasis::OasisBoxF& target);

    void set_size(unsigned int width, unsigned int height);

    void set_box(const Oasis::OasisBoxF& target);

    void set_trans(const Oasis::OasisTrans& trans);

    unsigned int width() const
    {
        return m_width;
    }

    unsigned int height() const
    {
        return m_height;
    }

    const Oasis::OasisTrans& trans() const
    {
        return m_trans;
    }

    Oasis::OasisBoxF box() const;

    Oasis::OasisBoxF target_box() const
    {
        return m_target_box;
    }

private:
    unsigned int m_width, m_height;
    Oasis::OasisTrans m_trans;
    Oasis::OasisBoxF m_target_box;
};

}

#endif // RENDER_VIEWPORT_H
