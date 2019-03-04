#include "qtlogger.h"
namespace Log4Qt
{

LoggerFile* LoggerFile::m_instance = NULL;
LoggerConsole * LoggerConsole :: m_instance = NULL;
QString LoggerFile::m_filter = "";
QString LoggerConsole :: m_filter = "";

LoggerFile::LoggerFile(QObject *parent)
{
    setParent(parent);
    m_file_path = QDir::homePath() + "/.pangen_gui" + "/pgui_log/";
    init_file_setting();
    Log4Qt::PropertyConfigurator::configure(*m_file_setting);
    init_file_logger();
}

LoggerFile::~LoggerFile()
{
    delete m_instance;
    m_instance = NULL;
}

LoggerFile *LoggerFile::get_instance(const char *fileName, int lineNumber, const char *functionName)
{
    m_filter = QString("[FILE:%1, LINE(%2)]: ").arg(fileName).arg(lineNumber);
    QMutex m_mutex;
    m_mutex.lock();
    if(NULL == m_instance)
    {
        m_instance = new LoggerFile();
    }
    m_mutex.unlock();
    return m_instance;
}

LoggerFile *LoggerFile::get_instance()
{
    QMutex m_mutex;
    m_mutex.lock();
    if(NULL == m_instance)
    {
        m_instance = new LoggerFile();
    }
    m_mutex.unlock();
    return m_instance;
}

void LoggerFile::init_file_setting()
{
    QString configFile_path = QDir::homePath()+ "/.pangen_gui" + "/pgui_config";
    QDir dir(configFile_path);
    if (!dir.exists())
    {
        if(!dir.mkpath(configFile_path))
        {
            qDebug() << "make config_dir error !";
            return;
        }
    }
    m_file_setting = new QSettings(configFile_path + "/qtlogger_file.conf", QSettings::IniFormat);
    m_file_setting->clear();
    m_file_setting->setValue("log4j.rootLogger", "debug,dailyFile");
    m_file_setting->setValue("log4j.appender.dailyFile", "org.apache.log4j.DailyRollingFileAppender");
    m_file_setting->setValue("log4j.appender.dailyFile.File", m_file_path + QDateTime::currentDateTime().toString("yyyyMMdd")+ ".log");
    m_file_setting->setValue("log4j.appender.dailyFile.AppendFile", "true");
    m_file_setting->setValue("log4j.appender.dailyFile.layout", "org.apache.log4j.PatternLayout");
    m_file_setting->setValue("log4j.appender.dailyFile.layout.ConversionPattern", "%-d{yyyy-MM-dd HH:mm:ss.zzz} [%p] %m%n");
}

void LoggerFile::init_file_logger()
{
    QDir Log_Dir(m_file_path);
    if (!Log_Dir.exists())
    {
        if (Log_Dir.mkpath(m_file_path))
        {
            qDebug() << "make log file path error!";
        }
    }
    m_file_logger = Log4Qt::Logger::logger("File");
}

void LoggerFile::debug(const QVariant& Text)
{
    m_file_logger->debug(m_filter + Text.toString());
}

void LoggerFile::waring(const QVariant& Text)
{
    m_file_logger->warn(m_filter + Text.toString());
}

void LoggerFile::info(const QVariant& Text)
{
    m_file_logger->info(m_filter + Text.toString());
}

void LoggerFile::error(const QVariant& Text)
{
    m_file_logger->error(m_filter + Text.toString());
}

LoggerConsole::LoggerConsole(QObject *parent):
       QObject(parent)
{
    init_console_setting();
    Log4Qt::PropertyConfigurator::configure(*m_console_setting);
    Log4Qt::LogManager::setHandleQtMessages(true);
    init_console_logger();
}

LoggerConsole::~LoggerConsole()
{
    delete m_instance;
    m_instance = NULL;
}

void LoggerConsole::init_console_setting()
{
   QString configFile_path = QDir::homePath()+ "/.pangen_gui" + "/pgui_config";
   QDir dir(configFile_path);
   if (!dir.exists())
   {
       if(!dir.mkpath(configFile_path))
       {
           qDebug() << "make config_dir error !";
           return;
       }
   }
   m_console_setting = new QSettings(configFile_path + "/qtlogger_console.conf", QSettings::IniFormat);
   m_console_setting->clear();
   m_console_setting->setValue("log4j.rootLogger", "DEBUG, console");
   m_console_setting->setValue("log4j.appender.console", "org.apache.log4j.ConsoleAppender");
   m_console_setting->setValue("log4j.appender.console.layout", "org.apache.log4j.PatternLayout");
   m_console_setting->setValue("log4j.appender.console.layout.ConversionPattern", "%-d{yyyy-MM-dd HH:mm:ss.zzz} [%p] %m%n");
}

const LoggerConsole&  LoggerConsole::operator<<(QVariant data)
{
    debug(data);
    return *this;
}


void LoggerConsole::init_console_logger()
{
       m_console_logger = Log4Qt::Logger::logger("Debug");
}

LoggerConsole *LoggerConsole::get_instance(const char *fileName, int lineNumber, const char *functionName)
{
    m_filter = QString("[FILE:%1, LINE(%2)]: ").arg(fileName).arg(lineNumber);
    QMutex m_mutex;
    m_mutex.lock();
    if(NULL == m_instance)
    {
        m_instance = new LoggerConsole();
    }
    m_mutex.unlock();
    return m_instance;
}

void LoggerConsole::debug(const QVariant &Text)
{
    m_console_logger->debug(m_filter + Text.toString());
}

void LoggerConsole::waring(const QVariant &Text)
{
    m_console_logger->warn(m_filter + Text.toString());
}

void LoggerConsole::info(const QVariant &Text)
{
    m_console_logger->info(m_filter + Text.toString());
}

void LoggerConsole::error(const QVariant &Text)
{
    m_console_logger->error(m_filter + Text.toString());
}

//QString LoggerConsole::variant_to_string(const QVariant & variant)
//{
//}

}


