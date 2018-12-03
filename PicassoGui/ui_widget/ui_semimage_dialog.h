#ifndef SEMIMAGEDIALOG_H
#define SEMIMAGEDIALOG_H
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QLayout>
namespace  ui {
class SemImageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SemImageDialog(QWidget *parent = 0);

signals:

public slots:

private:
    QLabel *fileLabel;
    QLineEdit *fileEdit;
    QPushButton *fileButton;
    QCheckBox *moveCheckBox;
    QCheckBox *dismissCheckBox;
    QLabel *widthLabel;
    QLabel *heightLabel;
    QLabel *centerLabel;
    QLabel *yLabel;
    QLineEdit *widthEdit;
    QLineEdit *heightEdit;
    QLineEdit *centerXEdit;
    QLineEdit *yEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;
};
}
#endif // SEMIMAGEDIALOG_H
