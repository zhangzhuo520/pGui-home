#ifndef UI_RTSMASK_TAB_H
#define UI_RTSMASK_TAB_H
#include <QTabWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>

namespace ui {
class MaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaskWidget(QWidget * parent = 0);
    ~MaskWidget();

    void init_ui();
private:
    QPushButton *m_mask_add_button;
    QPushButton *m_mask_clone_button;
    QPushButton *m_mask_delete_button;
    QTableWidget *m_layer_table;
    QLabel *m_boolean_label;
    QLineEdit *m_boolean_edit;
};

class RtsMaskTab : public QTabWidget
{
    Q_OBJECT
public:
    explicit RtsMaskTab(QWidget *parent = 0);
    void init_tab(const QStringList &);
    void delete_all_tab();
    ~RtsMaskTab();

private:
    MaskWidget * m_mask_widget;

};

}
#endif // UI_RTSMASKTAB_H
