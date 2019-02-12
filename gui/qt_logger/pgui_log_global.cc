#include "pgui_log_global.h"

namespace ui
{

WidgetLogger * WidgetLogger::m_instance = NULL;

WidgetLogger *WidgetLogger::get_instance()
{
     QMutex m_mutex;
     m_mutex.lock();
     if(NULL == m_instance)
     {
         m_instance = new WidgetLogger();
     }
     m_mutex.unlock();
     return m_instance;
}


LogAppender::LogAppender(QObject *parent) :
        Log4Qt::AppenderSkeleton(parent)
{
}

LogAppender::~LogAppender()
{
}

void LogAppender::init_logger()
{
}

bool LogAppender::requiresLayout() const
{
        return true;
}

void LogAppender::append(const Log4Qt::LoggingEvent &rEvent)
{
        Q_ASSERT_X(layout(), "LogViewAppender::append()", "Layout must not be null");
        QString message(layout()->format(rEvent));
        emit signal_append_measasge(message);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug LogAppender::debug(QDebug &rDebug) const
{
        return rDebug.space();
}
#endif //QT_NO_DEBUG_STREAM


WidgetLogger::WidgetLogger()
{
    // nothing else...
}

WidgetLogger::~WidgetLogger()
{
    delete m_view_appender;
    delete m_view_layout;
    delete m_instance;
    m_instance = NULL;
    m_view_appender = NULL;
    m_view_layout = NULL;
}

void WidgetLogger::init_logger()
{
    m_widget_logger = Log4Qt::Logger::rootLogger();
    m_view_appender = new LogAppender();

    m_view_appender->setName("ViewAppender");

    m_view_layout= new Log4Qt::PatternLayout(Log4Qt::PatternLayout::TTCC_CONVERSION_PATTERN);
    m_view_layout->setConversionPattern("[%-d{yyyy-MM-dd HH:mm:ss.zzz}]:  %m%n");
    m_view_appender->setLayout(m_view_layout);
    m_view_appender->activateOptions();
    m_widget_logger->addAppender(m_view_appender);

    //this has a question, Except for Maindow.cc, other file use logger, there show no m_view_appender...
    connect(m_view_appender, SIGNAL(signal_append_measasge(QString)), this, SLOT(slot_append_message(QString)));
}

void WidgetLogger::debug(const QVariant& Text)
{
   QString time_s = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + " " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "] :";
   emit signal_append_measasge(time_s + Text.toString());
}

LogAppender *WidgetLogger::get_appender()
{
    return m_view_appender;
}

void WidgetLogger::slot_append_message(QString text)
{
    emit signal_append_measasge(text);
}

}
