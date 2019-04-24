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
class QWidget;
namespace ui {
class RtsRunProcess : public QObject
{
    Q_OBJECT
public:
    explicit RtsRunProcess(QWidget *parent = 0);
    ~RtsRunProcess();

    void run();

    void stop();

    void set_script_path(const QString&);
signals:
    void signal_rtsrun_finished();
    void signal_rtsrun_error(const QString&);

public slots:
    void slot_process_start();

    void slot_process_end(int);

    void slot_read_error();

    void slot_read_output();

private:
    QProcess* m_run_process;
    QString m_script_path;
    QWidget *m_parent_widget;
};
}
#endif // RTS_RUNPROCESS_H
