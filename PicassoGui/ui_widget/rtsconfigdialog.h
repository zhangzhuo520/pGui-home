#ifndef RTSCONFIGWIDGET_H
#define RTSCONFIGWIDGET_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QFrame>
#include "./deftools/defcontrols.h"
const int LableWidth = 115;
const int LableTitilWidth = 50;
const int pushButtonWidth = 40;
namespace UI {
class RtsConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RtsConfigDialog(QWidget *parent = 0);

    ~RtsConfigDialog();

    void initRtsWidget();

    void initTopButton();

    void initTabWidget();

    void initBottomButton();
    
signals:
    
public slots:
    void slotsAddRts();

    void slotsDeleteRts();

    void slotsReorder();
private:
    QPushButton * addRtsButton;
    QPushButton *deleteRtsButton;
    QPushButton *reorderButton;
    QPushButton *workButton;
    QPushButton *xCorrectionButton;
    QPushButton *FlareButton;
    QPushButton *ViewButton;
    QPushButton * okButton;
    QPushButton *cancelButton;
    QPushButton *applyButton;
    QTabWidget* rtsTabWidget;
    QWidget *rtsWidget;
    QPushButton *modelButton;
    Commbox *modelCommbox;
    Commbox *defocusCommbox;
    QCheckBox *useGPUcheckbox;
    QCheckBox * berderCheckbox;
    QRadioButton *mapRadioButton;
    QRadioButton *constantRadioButton;
    QFrame * maskFrame;
    QLineEdit *maskBiasEidt;
    QLineEdit *DeltaDoseEdit;
    QLineEdit *WorkingEdit;
    QLineEdit *xCorrectionEdit;
    QLineEdit *flareEdit;
    QLineEdit *mapOffsetEdit;
    QLineEdit *constantEdit;

    QSpinBox *sampSpinBox;

    QHBoxLayout *Hlayout;
    QVBoxLayout *Vlayout;
    QVBoxLayout *tabVlayout;
    QHBoxLayout *Hlayout1;
};
}
#endif // RTSCONFIGWIDGET_H
