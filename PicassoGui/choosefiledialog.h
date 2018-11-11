#ifndef CHOOSEFILEDIALOG_H
#define CHOOSEFILEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>

namespace UI{
class ChooseFileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChooseFileDialog(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QLabel *gdsFileLabel;
    QLineEdit *gdsFileEdit;
    QPushButton *gdsFileButton;

    QLabel *dataTypeLabel;
    QLineEdit *dataTypeEdit;
    QLabel *layerLabel;
    QLineEdit *layerEdit;
    QLabel *gaugeFileLabel;
    QLineEdit *gaugeEdit;
    QPushButton *gaugeButton;

    QPushButton *okButton;
    QPushButton *cancelButton;
};
}
#endif // CHOOSEFILEDIALOG_H
