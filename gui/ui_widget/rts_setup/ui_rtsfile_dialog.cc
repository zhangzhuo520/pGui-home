#include "ui_rtsfile_dialog.h"
namespace ui {

RtsFileDialog::RtsFileDialog(QWidget *parent):
    QDialog(parent)
{
    m_rts_fileedit = new RtsFileEdit(this);
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setSpacing(0);
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(m_rts_fileedit);
    setLayout(hlayout);

    m_rts_fileedit->set_mode(RtsFileEdit::EDIT);
    HighLighter *highlighter = new HighLighter(m_rts_fileedit->document());
//    Q_UNUSED(highlighter);
}

RtsFileDialog::~RtsFileDialog()
{
}
}
