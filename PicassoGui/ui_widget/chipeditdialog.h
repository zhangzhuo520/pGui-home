#ifndef CHIPEDITDIALOG_H
#define CHIPEDITDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
namespace UI {
class chipEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit chipEditDialog(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QLabel *configLabel;
    QLabel *showMessageLabel;
    QLineEdit *configEdit;
    QTextEdit *configTextEdit;
    QPushButton *openButton;
    QPushButton *saveButton;
    QPushButton *saveasButton;
    QPushButton *formatButton;
    QPushButton *previewButton;
    QPushButton *generateButton;
    QPushButton *closeButton;
};
}
#endif // CHIPEDITDIALOG_H
