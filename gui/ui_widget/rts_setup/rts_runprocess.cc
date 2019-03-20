#include "rts_runprocess.h"
namespace ui {

RtsRunProcess::RtsRunProcess(QObject *parent):
    QObject(parent)
{
    m_run_process = new QProcess(this);
    connect(m_run_process, SIGNAL(started()), this, SLOT(slot_process_start()));
    connect(m_run_process, SIGNAL(finished(int)), this, SLOT(slot_process_end(int)));
}

RtsRunProcess::~RtsRunProcess()
{
}

void RtsRunProcess::run()
{
    QString app_path = QDir::homePath() + "/.pangen_gui/pgui_rts/run.sh";
    QFile file(app_path);
    if (file.exists())
    {
        const QString str = QDir::homePath() + "/.pangen_gui/pgui_rts";
        QDir::setCurrent(str);
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

void RtsRunProcess::slot_process_start()
{
    logger_widget("The script is running, Please wait a moment!");
}

void RtsRunProcess::slot_process_end(int exitCode)
{
    Q_UNUSED(exitCode);
    emit signal_rtsrun_finished();
    logger_widget ("The script has run out!");
}
}
