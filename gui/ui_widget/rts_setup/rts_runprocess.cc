#include "rts_runprocess.h"
#include <QWidget>
namespace ui {

RtsRunProcess::RtsRunProcess(QWidget *parent):
    m_parent_widget(parent)
{
    m_run_process = new QProcess(this);
    connect(m_run_process, SIGNAL(started()), this, SLOT(slot_process_start()));
    connect(m_run_process, SIGNAL(readyReadStandardError()), this, SLOT(slot_read_error()));
    connect(m_run_process, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_read_output()));
    connect(m_run_process, SIGNAL(finished(int)), this, SLOT(slot_process_end(int)));
}

RtsRunProcess::~RtsRunProcess()
{
}

void RtsRunProcess::run()
{
    if(m_script_path.isEmpty())
    {
        logger_widget("Script path is empty!");
    }
    QString app_path = m_script_path + "/run.sh";
    QFile file(app_path);
    if (file.exists())
    {
        QDir::setCurrent(m_script_path);
        m_run_process->start("bash run.sh");
    }
    else
    {
        logger_console << "No script file!";
    }
}

void RtsRunProcess::stop()
{
    m_run_process->close();
}

void RtsRunProcess::set_script_path(const QString & path)
{
    m_script_path = path;
}

void RtsRunProcess::slot_process_start()
{
    logger_widget("The script is running, Please wait a moment!");
}

void RtsRunProcess::slot_process_end(int exitCode)
{
    if (exitCode == 0)
    {
        emit signal_rtsrun_finished();
    }
}

void RtsRunProcess::slot_read_error()
{
    QByteArray byte_array = m_run_process->readAllStandardError();
    QString error;
    error = byte_array;
    logger_widget(byte_array);
    showWarning(m_parent_widget, "Warning", "Rts run error!");
    m_run_process->close();
    emit signal_rtsrun_error(error);
}

void RtsRunProcess::slot_read_output()
{
    QByteArray byte_array = m_run_process->readAllStandardOutput();
    logger_widget(byte_array);
}
}
