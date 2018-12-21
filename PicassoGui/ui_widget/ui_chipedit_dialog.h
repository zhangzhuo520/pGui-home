#ifndef CHIPEDITDIALOG_H
#define CHIPEDITDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
namespace ui {
class chipEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit chipEditDialog(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QLabel *m_config_label;
    QLabel *m_message_label;
    QLineEdit *m_config_edit;
    QTextEdit *m_config_textedit;
    QPushButton *m_open_button;
    QPushButton *m_save_button;
    QPushButton *m_saveas_button;
    QPushButton *m_forma_button;
    QPushButton *m_preview_button;
    QPushButton *m_generate_button;
    QPushButton *m_close_button;
};
}
#endif // CHIPEDITDIALOG_H
