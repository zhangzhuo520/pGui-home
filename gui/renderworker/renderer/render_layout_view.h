#ifndef RENDER_LAYOUT_VIEW_H
#define RENDER_LAYOUT_VIEW_H

#include "oasis_layout.h"
#include <memory>

namespace render{

class RenderFrame;

class LayoutView
{
public:

    LayoutView();

    LayoutView(int index, oasis::OasisLayout* layout, RenderFrame* widget);

    LayoutView& operator= (const LayoutView& );

    bool operator==(const LayoutView& lv);

    oasis::OasisLayout* get_layout() const
    {
        return m_layout.get();
    }

    void set_layout(oasis::OasisLayout* layout)
    {
        m_layout.reset(layout);
    }

    int index() const
    {
        return m_index;
    }

    void set_index(int index)
    {
        m_index = index;
    }

    RenderFrame* get_widget() const
    {
        return m_widget;
    }

    void set_widget(RenderFrame* widget)
    {
        m_widget = widget;
    }

    void attach(render::RenderFrame* frame, std::string prep_dir, bool add_layout_view);

    void detach();

    const std::string& file_name() const
    {
        return m_file_name;
    }

    void set_file_name(std::string& file_name)
    {
        m_file_name = file_name;
    }

    oasis::float64 get_dbu() const
    {
        oasis::OasisLayout* p = m_layout.get();
        if(p)
        {
            return p->get_dbu();
        }
        else
        {
            return 0.0001;
        }
    }

    int get_count() const
    {
        return m_layout.use_count();
    }

    bool valid() const
    {
        return m_valid;
    }

    void set_valid(bool valid)
    {
        m_valid = valid;
    }

    bool enable_attach() const
    {
        return m_enable_attach;
    }

    void set_enable_attach(bool attach)
    {
        m_enable_attach = attach;
    }

private:
    friend class RenderFrame;

    RenderFrame* m_widget;
    std::shared_ptr<oasis::OasisLayout> m_layout;
    int m_index;

    std::string m_file_name;

    bool m_valid;
    bool m_enable_attach;
};


}
#endif // RENDER_LAYOUT_VIEW_H
