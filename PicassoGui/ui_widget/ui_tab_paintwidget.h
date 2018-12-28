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

    ~TabPaintWidget();

public slots:
    void slot_close_tab(QString);

    void creat_canvas();

    void slot_show_measure_table();

    void slot_set_line_list(const QList <LineData>&);

    render::LayoutView load_file(const QString &, const QString &, bool);

signals:
    void signal_close_tab(QString);

private:
    void init_measure_table();
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
