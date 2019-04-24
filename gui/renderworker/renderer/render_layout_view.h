#ifndef RENDER_LAYOUT_VIEW_H
#define RENDER_LAYOUT_VIEW_H

#include "oasis_layout.h"
#include <memory>
#include <stdexcept>

namespace render{

class RenderFrame; 
class LayoutViewProxy;

class append_exception: public std::logic_error
{
public:
    explicit append_exception(const std::string& s): std::logic_error(s) { }

    append_exception(const std::string &s, render::RenderFrame* frame): std::logic_error(s), m_frame(frame) { }

    const render::RenderFrame* m_frame;
};

class append_error:public std::logic_error
{
public:
    explicit append_error(const std::string& s): std::logic_error(s) {}

};

class LayoutView
{
public:

    LayoutView();

    LayoutView(oasis::OasisLayout* layout);

    LayoutView& operator= (const LayoutView& );

    ~LayoutView();

    bool operator==(const LayoutView& lv);

    oasis::OasisLayout* get_layout() const
    {
        return m_layout;
    }

    void set_layout(oasis::OasisLayout* layout);

    void attach(render::RenderFrame* frame, std::string prep_dir, bool add_layout_view);

    void close();

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
        oasis::OasisLayout* p = m_layout;
        if(p)
        {
            return p->get_dbu();
        }
        else
        {
            return 0.0001;
        }
    }

    bool valid() const
    {
        return m_valid;
    }

    void set_valid(bool valid)
    {
        m_valid = valid;
    }

    void erase_proxy(LayoutViewProxy& lv);

    void load_into_frame(render::RenderFrame* frame, std::string prep_dir);

    bool has_single_view();

    render::RenderFrame* single_view();



private:
    friend class RenderFrame;

    oasis::OasisLayout* m_layout;

    std::string m_file_name;

    bool m_valid;

    std::vector<LayoutViewProxy> m_proxys;
};

class LayoutViewProxy
{
public:
    LayoutViewProxy()
    {
        m_layout_view = 0;
        m_frame = 0;
    }

    LayoutViewProxy(LayoutView* lv, RenderFrame* frame);

    int index() const;

    RenderFrame* frame();

    bool operator== (const LayoutView* lv) const;

    bool operator!= (const LayoutView* lv) const
    {
        return !operator== (lv);
    }

    bool operator== (const LayoutViewProxy& lv_proxy) const
    {
        return m_layout_view == lv_proxy.m_layout_view && m_frame == lv_proxy.m_frame;
    }

    bool operator!= (const LayoutViewProxy& lv_proxy) const
    {
        return !operator==(lv_proxy);
    }

    LayoutView* operator->() const;

    bool is_valid() const;

private:
    friend class RenderFrame;
    LayoutView* m_layout_view;
    RenderFrame*  m_frame;
};

}
#endif // RENDER_LAYOUT_VIEW_H
