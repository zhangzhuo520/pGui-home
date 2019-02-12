#ifndef UI_RTSMASK_TAB_H
#define UI_RTSMASK_TAB_H
#include <QTabWidget>
#include <QPushButton>

namespace ui {

class RtsMaskTab : public QTabWidget
{
    Q_OBJECT
public:
    explicit RtsMaskTab(QWidget *parent = 0);
    ~RtsMaskTab();

private:
    QPushButton *m_mask_add_button;
    QPushButton *m_mask_clone_button;
    QPushButton *m_mask_delete_button;
};

}
#endif // UI_RTSMASKTAB_H
