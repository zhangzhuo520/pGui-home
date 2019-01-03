#ifndef CMESSAGEBOX_H
#define CMESSAGEBOX_H

#include <QDialog>
#include <QPoint>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMouseEvent>

namespace ui {
class CMessageBox : public QDialog
{
    Q_OBJECT
public:
    explicit CMessageBox(QWidget *parent = 0,
            const QString &title = tr("Tip"),
            const QString &text = "",
            QMessageBox::StandardButtons buttons = QMessageBox::Ok,
            QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

    QAbstractButton *clickedButton() const;
    QMessageBox::StandardButton standardButton(QAbstractButton *button) const;
    void setDefaultButton(QPushButton *button);
    void setDefaultButton(QMessageBox::StandardButton defaultButton );
    void setIcon(const QString &icon);
    void setText(QString &text);

signals:

public slots:
    void slot_onButtonClicked(QAbstractButton *button);

private:
    int execReturnCode(QAbstractButton *button);

    QLabel *m_lIconTitle;
    QLabel *m_lTitle;
    QLabel *m_lIconMain;
    QLabel *m_lText;
    QVBoxLayout *m_layout;
    QDialogButtonBox *m_pButtonBox;
    QAbstractButton *m_pClickedButton;
};

/*********************************************************************************/
QMessageBox::StandardButton showError(QWidget *parent = 0,
                                      const QString &title = "Tip",
                                      const QString &text = "",
                                      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                      QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

QMessageBox::StandardButton showSuccess(QWidget *parent = 0,
                                        const QString &title = "Tip",
                                        const QString &text = "",
                                        QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                        QMessageBox::StandardButton defaultButton = QMessageBox::Ok);
QMessageBox::StandardButton showQuestion(QWidget *parent = 0,
                                         const QString &title = "Tip",
                                         const QString &text = "",
                                         QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                         QMessageBox::StandardButton defaultButton = QMessageBox::Ok);
QMessageBox::StandardButton showWarning(QWidget *parent = 0,
                                        const QString &title = "Tip",
                                        const QString &text = "",
                                        QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                        QMessageBox::StandardButton defaultButton = QMessageBox::Ok);
}
#endif // CMESSAGEBOX_H
