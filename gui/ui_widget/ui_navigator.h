#ifndef UI_NAVIGATOR_H
#define UI_NAVIGATOR_H
#include <QWidget>

namespace render {
class RenderFrame;
}

namespace ui {
class MainWindow;
class Navigator : public QWidget
{
    Q_OBJECT
public:
    explicit Navigator(MainWindow *mainwindow = 0, render::RenderFrame *src_render_frame = 0);
    ~Navigator();
    void update_source_frame(render::RenderFrame *);

    void update_frame_data();

private:
    MainWindow *m_mainwindow;
    render::RenderFrame *m_render_frame;
    render::RenderFrame *m_src_render_frame;
};
}
#endif // UI_NAVIGATOR_H
