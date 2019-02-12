#include "ui_log_widget.h"

namespace ui {


LogWidget::LogWidget(QWidget* parent)
        :QWidget(parent)
{
        QVBoxLayout *vlayout = new QVBoxLayout;
        setLayout(vlayout);
//        qRegisterMetaType<QTextCursor>("QTextCursor");
        m_view = new QTextBrowser(this);
        vlayout->setSpacing(0);
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->addWidget(m_view);
}

LogWidget::~LogWidget()
{

}

void LogWidget::slot_append_log(const QString &Message)
{
 //        QMutexLocker lock(&m_mutex);
 //        m_view->insertPlainText(Message);
 //        m_view->moveCursor(QTextCursor::End);
 //        m_view->ensureCursorVisible();
     m_view->append(Message);
}

}
