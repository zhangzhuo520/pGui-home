#ifndef UI_TABPAINTWIDGET_H
#define UI_TABPAINTWIDGET_H
#include <QTabWidget>
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

    void append_canvas(QString);

    void set_active_widget(QString);

    void update_measuretable_data();

    ~TabPaintWidget();

    void get_canvas_coord(double *, double *, double *, double *);

public slots:
    void slot_close_tab(QString);

    ScaleFrame* creat_canvas();

    void slot_show_measure_table();

    void slot_set_line_list(const QList <LineData>&);

//    void load_layout_view(render::LayoutView*, const QString &, bool);

//    void add_layout_view(render::LayoutView* lv, bool add_layout_view);

    void slot_layout_view_changed(render::RenderFrame* );

signals:
    void signal_close_tab(QString);

    void signal_set_line_list(const QList<LineData> &);

    void signal_layout_view_changed(render::RenderFrame*);
private:
    void init_measure_table();

    void init_connection();

    void connect_layer_widget();

    QString database_to_oas(QString);
    int string_to_index(QString);

    //  ScaleFrame *m_scaleframe;

    QVector <ScaleFrame *> m_scaleframe_vector;

    QWidget *m_mainwindow;

    DockWidget *m_measure_dockwidget;

    MeasureTable *m_measure_table;
};

}
#endif // UI_TABPAINTWIDGET_H
