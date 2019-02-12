#ifndef QTLOGGER_H
#define QTLOGGER_H

#include <QObject>
#include <QDateTime>
#include <QTextCodec>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QThread>
#include <QSettings>
#include <QMutex>
#include <QDebug>

#include "log4qt/logger.h"
#include "log4qt/consoleappender.h"
#include "log4qt/patternlayout.h"
#include "log4qt/fileappender.h"
#include "log4qt/basicconfigurator.h"
#include "log4qt/ttcclayout.h"
#include "log4qt/ndc.h"
#include "log4qt/propertyconfigurator.h"
#include "log4qt/logmanager.h"

namespace Log4Qt {

class LoggerFile : public QObject
{
    Q_OBJECT
public:
    explicit LoggerFile(QObject *parent = 0);
    ~LoggerFile();

    static LoggerFile *get_instance(const char *, int, const char *);
    static LoggerFile *get_instance();

    void init_file_setting();
    void init_file_logger();

    void debug(const QVariant&);
    void waring(const QVariant&);
    void info(const QVariant&);
    void error(const QVariant&);

private:
    Log4Qt::Logger *m_file_logger;
    QString m_file_path;
    QSettings *m_file_setting;
    static LoggerFile *m_instance;
    static QString m_filter;
};

class LoggerConsole : public QObject
{
    Q_OBJECT
public:
    explicit LoggerConsole(QObject *parent = 0);
    ~LoggerConsole();

    const LoggerConsole& operator<<(QVariant);

    void init_console_setting();
    void init_console_logger();

    static LoggerConsole *get_instance(const char *, int, const char *);
    void debug(const QVariant&);
    void waring(const QVariant&);
    void info(const QVariant&);
    void error(const QVariant&);
//    QString variant_to_string(const QVariant&);

private:
    Log4Qt::Logger *m_console_logger;
    QSettings *m_console_setting;
    static LoggerConsole *m_instance;
    static QString m_filter;
};

//class WidgetLogger : public QObject
//{
//    Q_OBJECT
//public:
//    explicit WidgetLogger(QObject *parent = 0);
//    ~WidgetLogger();

//    void init_widget_setting();
//    void init_widget_logger();

//    static LoggerConsole *get_instance(const char *, int, const char *);
//    void debug(const QVariant&);

//private:
//    Log4Qt::Logger *m_widget_logger;
//    QSettings *widget_setting;
//};
}
#endif // QTLOGGER_H
