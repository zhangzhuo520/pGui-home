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

    LayoutView(int index, Oasis::OasisLayout* layout, RenderFrame* widget);

    LayoutView& operator= (const LayoutView& );

    Oasis::OasisLayout* get_layout() const
    {
        return m_layout.get();
    }

    void set_layout(Oasis::OasisLayout* layout)
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

    void append(LayoutView* view);

    void detach();

    std::string file_name() const
    {
        return m_file_name;
    }

    void set_file_name(std::string& file_name)
    {
        m_file_name = file_name;
    }

private:
    friend class RenderFrame;

    RenderFrame* m_widget;
    std::shared_ptr<Oasis::OasisLayout> m_layout;
    int m_index;

    std::string m_file_name;

};


}
#endif // RENDER_LAYOUT_VIEW_H
