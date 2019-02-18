#ifndef PGUI_LOG_GLOBAL_H
#define PGUI_LOG_GLOBAL_H

#include <QDebug>
#include <QDateTime>
#include "qtlogger.h"
#include "log4qt/logger.h"
#include "log4qt/appenderskeleton.h"
#include "log4qt/appender.h"
#include "log4qt/patternlayout.h"

namespace ui
{

#ifndef logger_file
#define  logger_file(Value)         Log4Qt::LoggerFile::get_instance(__FILE__, __LINE__, Q_FUNC_INFO)->debug(Value);
#endif

//#ifndef logger_file_no
//#define  logger_file_no(Value)         Log4Qt::LoggerFile::get_instance()->debug(Value);
//#endif

#ifndef logger_widget
#define  logger_widget(Value)         WidgetLogger::get_instance()->debug(Value); Log4Qt::LoggerFile::get_instance(__FILE__, __LINE__, Q_FUNC_INFO)->debug(Value);
#endif

#ifndef logger_console
#define  logger_console      (*(Log4Qt::LoggerConsole::get_instance(__FILE__, __LINE__, Q_FUNC_INFO)))
#endif

class LogAppender : public Log4Qt::AppenderSkeleton
{
    Q_OBJECT
public:
        LogAppender(QObject *pParent = 0);
        ~LogAppender();
        void init_logger();

signals:
        void signal_append_measasge(const QString& msg);


protected:
        virtual bool requiresLayout() const;
        virtual void append(const Log4Qt::LoggingEvent &rEvent);


#ifndef QT_NO_DEBUG_STREAM
        virtual QDebug debug(QDebug &rDebug) const;
#endif //QT_NO_DEBUG_STREAM
};

class WidgetLogger : public QObject
{
    Q_OBJECT
public:
    explicit WidgetLogger();
    ~WidgetLogger();

    void init_logger();

    void debug(const QVariant&);

    LogAppender* get_appender();

    static WidgetLogger* get_instance();
signals:
    void signal_append_measasge(QString);

public slots:
    void slot_append_message(QString);

private:
    static WidgetLogger * m_instance;
    Log4Qt::Logger *m_widget_logger;
    LogAppender *m_view_appender;
    Log4Qt::PatternLayout *m_view_layout;
};

}
#endif // PGUI_LOG_GLOBAL_H
