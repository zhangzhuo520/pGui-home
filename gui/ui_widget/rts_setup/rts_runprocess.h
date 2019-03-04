#ifndef RTS_RUNPROCESS_H
#define RTS_RUNPROCESS_H
#include <QProcess>
#include <QObject>
#include <QDir>
#include <QLabel>
#include <QApplication>
#include <iostream>

#include "../deftools/cmessagebox.h"
#include "../qt_logger/pgui_log_global.h"
namespace ui {

class RtsRunProcess : public QObject
{
    Q_OBJECT
public:
    explicit RtsRunProcess(QObject *parent = 0);
    ~RtsRunProcess();

    void run();
signals:
    void signal_rtsrun_finished();

public slots:
    void slot_process_start();

    void slot_process_end(int);

private:
    QProcess* m_run_process;
};
}
#endif // RTS_RUNPROCESS_H
