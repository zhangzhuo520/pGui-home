#ifndef UI_RTSFILE_DIALOG_H
#define UI_RTSFILE_DIALOG_H
#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QKeyEvent>
#include <QDebug>
#include <QFont>

#include "ui_rtsfile_edit.h"
#include "highlighter.h"
#include "../../qt_logger/pgui_log_global.h"
namespace ui {

class RtsFileDialog : public QDialog
{
    Q_OBJECT
public:
    RtsFileDialog(QWidget *parent = 0);
    ~RtsFileDialog();

    void init_ui();

    void init_filedialog();

    void init_connection();

public slots:
    void slot_save_file();

    void slot_open_file();

    void slot_get_file(QString);

    void slot_text_change();

protected:
    void keyPressEvent(QKeyEvent *e)
    {
        if (e->modifiers() == (Qt::ControlModifier) && e->key() == Qt::Key_S)
        {
            slot_save_file();
        }
    }

private:
    QString read_file(const QString&);

    RtsFileEdit *m_rts_fileedit;
    QFileDialog *m_file_dialog;
    QPushButton *m_open_button;
    QPushButton *m_save_button;
    QString m_current_file;
    QString m_file_path;
};
}
#endif // UI_RTSFILE_DIALOG_H
