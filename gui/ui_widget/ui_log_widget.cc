#include "ui_log_widget.h"

namespace ui {


LogWidget::LogWidget(QWidget* parent)
        :QWidget(parent)
{
        QVBoxLayout *vlayout = new QVBoxLayout;
        setLayout(vlayout);
        m_text_browser = new QTextBrowser(this);
        vlayout->setSpacing(0);
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->addWidget(m_text_browser);
}

LogWidget::~LogWidget()
{

}

void LogWidget::slot_append_log(const QString &Message)
{
     m_text_browser->append(Message);
}

}
