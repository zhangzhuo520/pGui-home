#include "cmessagebox.h"
#include "QDebug"
namespace ui {
CMessageBox::CMessageBox(QWidget *parent,
                         const QString &title,
                         const QString &text,
                         QMessageBox::StandardButtons standButtons,
                         QMessageBox::StandardButton defaultButton
                         ) :
    QDialog(parent)
{
    setStyleSheet("QDialog{border:2px solid rgb(150,150,150) ;}"\
                  /*   "QPushButton{border:2px solid rgb(176,181,185);"\
                                            "border-radius:1px;}"*/);\

    m_lIconTitle = new QLabel(this);
    m_lTitle = new QLabel(this);
    m_lIconMain = new QLabel(this);
    m_lText = new QLabel(this);
    m_layout = new QVBoxLayout();
    m_pButtonBox = new QDialogButtonBox(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    setFixedSize(QSize(400, 200));
    m_layout->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(8, 0, 0, 0);
    titleLayout->setSpacing(5);
    titleLayout->addWidget(m_lIconTitle);
    titleLayout->addWidget(m_lTitle);
    titleLayout->addStretch(1);
    m_lIconTitle->setFixedSize(60, 45);
    m_lIconTitle->setPixmap(QPixmap(":/new/prefix1/images/dfjy.png"));
    m_lIconTitle->setScaledContents(true);
    m_lTitle->setFixedHeight(32);
    m_layout->addLayout(titleLayout);

    QGroupBox *groupBox = new QGroupBox(this);
    groupBox->setFixedHeight(140);
    m_layout->addWidget(groupBox);
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    groupBox->setLayout(vBoxLayout);
    vBoxLayout->setContentsMargins(10, 0, 10, 10);

    QHBoxLayout *hLayout  = new QHBoxLayout();
    m_lIconMain->setFixedSize(QSize(40, 40));
    m_lText->setFixedHeight(40);
    hLayout->addWidget(m_lIconMain);
    hLayout->addWidget(m_lText);
    hLayout->addStretch(1);
    vBoxLayout->addLayout(hLayout);

    QHBoxLayout *hLayoutButtons  = new QHBoxLayout();
    hLayoutButtons->addStretch(1);
    hLayoutButtons->addWidget(m_pButtonBox);
    vBoxLayout->addLayout(hLayoutButtons);
    m_pButtonBox->setFixedHeight(50);
    m_layout->addWidget(groupBox);
    setLayout(m_layout);

    m_pButtonBox->setStandardButtons(QDialogButtonBox::StandardButtons((int)standButtons));
    setDefaultButton(defaultButton);

    m_lTitle->setText(title);
    m_lText->setText(text);

    connect(m_pButtonBox, SIGNAL(clicked(QAbstractButton*)), this,
            SLOT(slot_onButtonClicked(QAbstractButton*)));
}

QMessageBox::StandardButton CMessageBox::standardButton(QAbstractButton *button) const
{
    return (QMessageBox::StandardButton)m_pButtonBox->standardButton(button);
}

QAbstractButton *CMessageBox::clickedButton() const
{
    return m_pClickedButton;
}

void CMessageBox::setDefaultButton(QPushButton *button)
{
    if(!m_pButtonBox->buttons().contains(button)) {
        return ;
    }
    button->setDefault(true);
    button->setFocus();
}

void CMessageBox::setDefaultButton(QMessageBox::StandardButton defaultButton)
{
    setDefaultButton(m_pButtonBox->button(QDialogButtonBox::StandardButton(defaultButton)));
}

int CMessageBox::execReturnCode(QAbstractButton *button)
{
    return m_pButtonBox->standardButton(button);
}

void CMessageBox::slot_onButtonClicked(QAbstractButton *button)
{
    m_pClickedButton = button;
    done(execReturnCode(button));
}

void CMessageBox::setIcon(const QString &icon)
{
    m_lIconMain->setStyleSheet(QString("border-image:url(%1)").arg(icon));
}

void CMessageBox::setText(QString &text)
{
     m_lText->setText(text);
}


/********************************************************************************************/
QMessageBox::StandardButton showError(QWidget *parent,
                                      const QString &title,
                                      const QString &text,
                                      QMessageBox::StandardButtons buttons,
                                      QMessageBox::StandardButton defaultButton)
{
    CMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/new/prefix1/images/error.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton showSuccess(QWidget *parent,
                                        const QString &title,
                                        const QString &text,
                                        QMessageBox::StandardButtons buttons,
                                        QMessageBox::StandardButton defaultButton)
{
    CMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/new/prefix1/images/info.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton showQuestion(QWidget *parent,
                                         const QString &title,
                                         const QString &text,
                                         QMessageBox::StandardButtons buttons,
                                         QMessageBox::StandardButton defaultButton)
{
    CMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/new/prefix1/images/question.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton showWarning(QWidget *parent,
                                        const QString &title,
                                        const QString &text,
                                        QMessageBox::StandardButtons buttons,
                                        QMessageBox::StandardButton defaultButton)
{
    CMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/new/prefix1/images/waring.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}
}
