#ifndef RENDER_VIEWPORT_H
#define RENDER_VIEWPORT_H
#include "oasis_types.h"

namespace render{

class Viewport
{
public:
    Viewport();

    Viewport(unsigned int width, unsigned int height, const oasis::BoxF& target);

    void set_size(unsigned int width, unsigned int height);

    void set_box(const oasis::BoxF& target);

    void set_trans(const oasis::OasisTrans& trans);

    unsigned int width() const
    {
        return m_width;
    }

    unsigned int height() const
    {
        return m_height;
    }

    const oasis::OasisTrans& trans() const
    {
        return m_trans;
    }

    oasis::BoxF box() const;

    oasis::BoxF target_box() const
    {
        return m_target_box;
    }

private:
    unsigned int m_width, m_height;
    oasis::OasisTrans m_trans;
    oasis::BoxF m_target_box;
};

}

#endif // RENDER_VIEWPORT_H
