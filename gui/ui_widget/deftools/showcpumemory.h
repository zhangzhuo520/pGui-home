#ifndef SHOWCPUMEMORY_H
#define SHOWCPUMEMORY_H

#include <QObject>
#include <QDebug>
#include <QFile>

class QLabel;
class QTimer;
class QProcess;

namespace ui {

class ShowCPUMemory : public QObject
{
    Q_OBJECT
public:
    explicit ShowCPUMemory(QObject *parent = 0);
    QString GetCPU();
    QString GetMemory();


private:
    int totalNew, idleNew, totalOld, idleOld;
    int cpuPercent;
    double m_cpu_total;
    double m_cpu_use;

    int memoryPercent;
    int memoryAll;
    int memoryUse;
    int memoryFree;

signals:

public slots:
};
}
#endif // SHOWCPUMEMORY_H
