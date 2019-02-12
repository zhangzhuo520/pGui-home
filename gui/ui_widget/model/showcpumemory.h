#ifndef SHOWCPUMEMORY_H
#define SHOWCPUMEMORY_H

#include <QObject>
#include <QDebug>

class QLabel;
class QTimer;
class QProcess;

namespace ui {

class ShowCPUMemory : public QObject
{
    Q_OBJECT
public:
    explicit ShowCPUMemory(QObject *parent = 0);

    void Start(int interval);
    void Stop();
    QString get_cup_info();

private:
    int totalNew, idleNew, totalOld, idleOld;
    int cpuPercent;

    int memoryPercent;
    int memoryAll;
    int memoryUse;
    int memoryFree;

    QString m_info;

    QTimer *timerCPU;       //定时器获取CPU信息
    QTimer *timerMemory;    //定时器获取内存信息
    QLabel *labCPUMemory;   //显示CPU内存信息的控件
    QProcess *process;


private slots:
    QString GetCPU();
    QString GetMemory();
    QString ReadData();

signals:

public slots:
};
}
#endif // SHOWCPUMEMORY_H
