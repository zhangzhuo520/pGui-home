#include "ui_rtsconfig_dialog.h"
namespace ui {

RtsConfigDialog::RtsConfigDialog(QWidget *parent) :
    QDialog(parent)
{
    initTopButton();
    initTabWidget();
    initBottomButton();
    initLayout();
    initConnecttion();
}

RtsConfigDialog::~RtsConfigDialog()
{
}

void RtsConfigDialog::initTopButton()
{
    m_add_button = new QPushButton("Add RTS", this);
    m_clone_button = new QPushButton("Clone RTS",this);
    m_delete_button = new QPushButton("Delete RTS", this);
    connect(m_add_button, SIGNAL(clicked()), this, SLOT(slotAddRts()));
    connect(m_clone_button, SIGNAL(clicked()), this, SLOT(slotCloneRts()));
    connect(m_delete_button, SIGNAL(clicked()), this, SLOT(slotDeleteRts()));
}

void RtsConfigDialog::initTabWidget()
{
    m_rts_tab = new RtsTabWidget(this);
    m_rts_tab->tab_bar()->setStyle(new RtsTabStyle);
    m_rts_tab->setObjectName("RtsTabWidget");
    QString tabTitle = "RTS_" + QString::number(0);
    RtsconfigWidget * rtswidget = new RtsconfigWidget(m_rts_tab->count(), this);
    connect(rtswidget, SIGNAL(signal_image_parse_finished(int)), this, SLOT(slot_image_parse_finished(int)));
    connect(rtswidget, SIGNAL(signal_process_error(QString)), this, SLOT(slot_process_error(QString)));
    rtswidget->set_layername_list(m_layerdata_list);
    m_rts_tab->addTab(rtswidget, tabTitle);
}

void RtsConfigDialog::initBottomButton()
{
    m_ok_button = new QPushButton("Ok", this);
    m_cancel_button = new QPushButton("Cancel",this);
    m_apply_button = new QPushButton("Apply",this);
}

void RtsConfigDialog::initLayout()
{
    Vlayout = new QVBoxLayout(this);
    Hlayout = new QHBoxLayout();
    Hlayout->setSpacing(10);
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->addWidget(new QLabel(""));
    Hlayout->addWidget(m_add_button);
    Hlayout->addWidget(m_delete_button);
    Hlayout->addWidget(m_clone_button);
    Hlayout->setStretch(0, 5);
    Hlayout->setStretch(1, 1);
    Hlayout->setStretch(2, 1);
    Hlayout->setStretch(3, 1);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    Hlayout->addWidget(m_rts_tab);
    Vlayout->addLayout(Hlayout);

    Hlayout = new QHBoxLayout();
    Hlayout->addSpacerItem(new QSpacerItem(200, 20, QSizePolicy::Preferred, QSizePolicy::Preferred));
    Hlayout->addWidget(m_ok_button);
    Hlayout->addWidget(m_cancel_button);
    Hlayout->addWidget(m_apply_button);
    Vlayout->addLayout(Hlayout);

    setLayout(Vlayout);
}

void RtsConfigDialog::initConnecttion()
{
    connect(m_ok_button, SIGNAL(clicked()), this, SLOT(slot_ok_button()));
    connect(m_cancel_button, SIGNAL(clicked()), this, SLOT(slot_cancel_button()));
    connect(m_apply_button, SIGNAL(clicked()), this, SLOT(slot_apply_button()));
}

void RtsConfigDialog::slotAddRts()
{
    QString tabTitle = "RTS_" + QString::number(m_rts_tab->count());
    RtsconfigWidget * rtswidget = new RtsconfigWidget(m_rts_tab->count(), this);
    connect(rtswidget, SIGNAL(signal_image_parse_finished(int)), this, SLOT(slot_image_parse_finished(int)));
    rtswidget->set_layername_list(m_layerdata_list);
    m_rts_tab->addTab(rtswidget, tabTitle);
}

void RtsConfigDialog::slotDeleteRts()
{
    QWidget *widget = m_rts_tab->widget(m_rts_tab->count() - 1);
    m_rts_tab->removeTab(m_rts_tab->count() - 1);
    delete widget;
    widget = NULL;
}

void RtsConfigDialog::slotCloneRts()
{
    RtsconfigWidget *widget = static_cast <RtsconfigWidget *> (m_rts_tab->currentWidget());
    RtsconfigWidget *rtswidget = widget->clone(m_rts_tab->count());
    connect(rtswidget, SIGNAL(signal_image_parse_finished(int)), this, SLOT(slot_image_parse_finished(int)));
    m_rts_tab->addTab(rtswidget, "RTS_" + QString::number(m_rts_tab->count()));
}

void RtsConfigDialog::slot_ok_button()
{
    emit signal_get_current_canvaspos();
    if (m_rts_tab->count() == 0)
    {
        logger_widget("Tab is empty!");
        return;
    }
    else
    {
        for (int i = 0; i < m_rts_tab->count(); i ++)
        {
            RtsconfigWidget *widget = static_cast <RtsconfigWidget *> (m_rts_tab->widget(i));
            widget->do_apply();
        }
        this->close();
    }
}

void RtsConfigDialog::slot_cancel_button()
{
    close();
}

void RtsConfigDialog::slot_apply_button()
{
    emit signal_get_current_canvaspos();
    if (m_rts_tab->count() == 0)
    {
        logger_widget("Tab is empty!");
        return;
    }
    else
    {
        RtsconfigWidget *widget = static_cast <RtsconfigWidget *> (m_rts_tab->currentWidget());
        widget->do_apply();
    }
}

void RtsConfigDialog::slot_image_parse_finished(int)
{
    m_parse_finished_number++;
    QVector <RtsReviewInfo> infoVector;
    if (m_parse_finished_number == m_rts_tab->count())
    {
        for (int i = 0; i < m_rts_tab->count(); i ++)
        {
            RtsconfigWidget * widget = static_cast <RtsconfigWidget *> (m_rts_tab->widget(i));
            infoVector.append(widget->get_rts_reviewinfo());
        }
        emit signal_rts_finished(infoVector);
    }
}

void RtsConfigDialog::slot_process_error(const QString & s)
{
    emit signal_rtsprocess_error(s);
}

void RtsConfigDialog::set_layername_list(const QStringList & list)
{
    m_layerdata_list = list;
    for (int i = 0; i < m_rts_tab->count(); i ++)
    {
        RtsconfigWidget *widget = static_cast <RtsconfigWidget *> (m_rts_tab->widget(i));
        widget->set_layername_list(m_layerdata_list);
    }
}

void RtsConfigDialog::set_canvas_pos(const double &left, const double &right, const double &top, const double &bottom)
{
    for (int i = 0; i < m_rts_tab->count(); i ++)
    {
        RtsconfigWidget * widget = static_cast <RtsconfigWidget *> (m_rts_tab->widget(i));
        widget->set_canvas_pos(left, right, top, bottom);
    }
}

void RtsConfigDialog::start()
{
    static int history_number = 0;
    history_number ++;
    m_parse_finished_number = 0;
    for (int i = 0; i < m_rts_tab->count(); i ++)
    {
        RtsconfigWidget *widget = static_cast <RtsconfigWidget *> (m_rts_tab->widget(i));
        widget->start(history_number);
    }
}

void RtsConfigDialog::update_job_commbox(QStringList list)
{
    for (int i = 0; i < m_rts_tab->count(); i ++)
    {
        RtsconfigWidget *widget = static_cast <RtsconfigWidget *> (m_rts_tab->widget(i));
        widget->update_job_commbox(list);
    }
}

}
