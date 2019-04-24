#ifndef UI_TABPAINTWIDGET_H
#define UI_TABPAINTWIDGET_H
#include <QTabWidget>
#include <QScrollArea>
#include "ui_measure_table.h"
#include "deftools/defcontrols.h"
#include "ui_scale_frame.h"

namespace ui {

class TabPaintWidget:public QTabWidget
{
    Q_OBJECT
public:
    explicit TabPaintWidget(QWidget *parent = 0);

    ScaleFrame *get_scaleframe(int);

    QVector<render::RenderFrame*> get_render_frame_list();

    ScaleFrame* creat_canvas();

    void append_canvas();

    void set_active_widget(render::RenderFrame*);

    void update_measuretable_data();

//    void set_scroll_move_center();

    ~TabPaintWidget();

    void get_canvas_coord(double *, double *, double *, double *);

public slots:
    void slot_close_tab(int index);
    void slot_show_measure_table();
    void slot_set_line_list(const QList <LineData*>&);
    void slot_layout_view_changed(render::RenderFrame* );
    void slot_move_center(QPointF);

signals:
    void signal_close_tab(QString);
    void signal_set_line_list(const QList<LineData*> &);
    void signal_layout_view_changed(render::RenderFrame*);
    void signal_current_frameinfo(const FrameInfo &);

private:
    void init_measure_table();

    int string_to_index(QString);

    //  ScaleFrame *m_scaleframe;

    QVector <ScaleFrame *> m_scaleframe_vector;

    QWidget *m_mainwindow;

    DockWidget *m_measure_dockwidget;

    MeasureTable *m_measure_table;
};

}
#endif // UI_TABPAINTWIDGET_H
