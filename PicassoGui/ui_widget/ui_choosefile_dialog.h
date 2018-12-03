#ifndef CHOOSEFILEDIALOG_H
#define CHOOSEFILEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>

namespace ui{
class ChooseFileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChooseFileDialog(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QLabel *m_gdsfile_label;
    QLineEdit *m_gdsfile_edit;
    QPushButton *m_gdsfile_button;

    QLabel *m_datatype_label;
    QLineEdit *dataTypeEdit;
    QLabel *m_layer_label;
    QLineEdit *m_layer_edit;
    QLabel *m_gaugfile_label;
    QLineEdit *m_gaugfile_edit;
    QPushButton *m_gaugfile_button;

    QPushButton *m_ok_button;
    QPushButton *m_cancel_button;
};
}
#endif // CHOOSEFILEDIALOG_H
