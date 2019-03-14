#ifndef UI_RTSFILE_DIALOG_H
#define UI_RTSFILE_DIALOG_H
#include <QDialog>
#include <QHBoxLayout>
#include "ui_rtsfile_edit.h"
#include "highlighter.h"
namespace ui {

class RtsFileDialog : public QDialog
{
    Q_OBJECT
public:
    RtsFileDialog(QWidget *parent = 0);
    ~RtsFileDialog();

private:
    RtsFileEdit *m_rts_fileedit;
};
}
#endif // UI_RTSFILE_DIALOG_H
