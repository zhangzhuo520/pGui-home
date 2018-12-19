#ifndef UI_TABPAINTWIDGET_H
#define UI_TABPAINTWIDGET_H
#include <QTabWidget>

#include "ui_scale_frame.h"

namespace ui {

class TabPaintWidget:public QTabWidget
{
    Q_OBJECT
public:
  explicit TabPaintWidget(QWidget *parent = 0);
  ~TabPaintWidget();

public slots:
  void slot_close_tab(QString);

  void creat_canvas(QString);

 render::LayoutView load_file(const QString &, const QString &, bool);

private:

  int string_to_index(QString);

  QStringList m_filename_list;

  ScaleFrame *scameframe;

};

}
#endif // UI_TABPAINTWIDGET_H
